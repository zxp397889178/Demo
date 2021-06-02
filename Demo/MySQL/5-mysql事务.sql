-- MySQL事务

-- mysql中，事务其实是一个最小的不可分割的单元，事务能够保证一个业务的完整性

-- 比如银行转账：
	a -> -100
	update user set money=money-100 where name='a';
	
	b -> +100
	update user set money=money+100 where name='b';
-- 实际程序中，如果只有一条语句执行成功了，而另外一条没有执行成功
-- 出现数据不一致的情况。

-- 多条SQL语句，可能会有要求要么同时成功，要么同时失败




-- mysql中如何控制事务？

-- 1.mysql中默认是开启事务的（自动提交）。
mysql> select @@autocommit;
+--------------+
| @@autocommit |
+--------------+
|            1 |
+--------------+

-- 默认事务开启的作用是什么？
	-- 当我们去执行一个sql语句的时候，效果会立即体现出来，且不能回滚

create database back;

create table user(
	id int primary key,
	name varchar(200),
	money int
);

insert into user values(1,'a',1000);

-- 事务回滚: 撤销sql语句的执行

rollback;
mysql> rollback;
Query OK, 0 rows affected (0.00 sec)


-- 设置mysql自动提交为false

set autocommit=0;

mysql> set autocommit=0;
Query OK, 0 rows affected (0.00 sec)

mysql> select @@autocommit;
+--------------+
| @@autocommit |
+--------------+
|            0 |
+--------------+

-- 上面的操作，关闭了mysql的自动提交（commit）

mysql> insert into user values(2,'b',1000);
Query OK, 1 row affected (0.00 sec)

-- 手动提交数据
mysql> commit;
Query OK, 0 rows affected (0.01 sec)

-- 再撤销，是不可以撤销的（持久性）
mysql> rollback;
Query OK, 0 rows affected (0.00 sec)



-- 自动提交  @@autocommit=1
-- 手动提交	 commit;
-- 事务回滚	 rollback;


-- 如果这时候转账：
	update user set money=money-100 where name='a';
	update user set money=money+100 where name='b';

mysql> update user set money=money-100 where name='a';
Query OK, 1 row affected (0.00 sec)
Rows matched: 1  Changed: 1  Warnings: 0

mysql> update user set money=money+100 where name='b';
Query OK, 1 row affected (0.00 sec)
Rows matched: 1  Changed: 1  Warnings: 0

mysql> select * from user;
+----+------+-------+
| id | name | money |
+----+------+-------+
|  1 | a    |   900 |
|  2 | b    |  1100 |
+----+------+-------+
2 rows in set (0.00 sec)

mysql> rollback;
Query OK, 0 rows affected (0.01 sec)

mysql> select * from user;
+----+------+-------+
| id | name | money |
+----+------+-------+
|  1 | a    |  1000 |
|  2 | b    |  1000 |
+----+------+-------+

-- 事务给我们提供了一个反悔的机会



begin;
-- 或者
start transaction;
-- 都可以帮助我们手动开启一个事务


mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> update user set money=money-100 where name='a';
Query OK, 1 row affected (0.00 sec)
Rows matched: 1  Changed: 1  Warnings: 0

mysql> update user set money=money+100 where name='b';
Query OK, 1 row affected (0.00 sec)
Rows matched: 1  Changed: 1  Warnings: 0

mysql> select * from user;
+----+------+-------+
| id | name | money |
+----+------+-------+
|  1 | a    |   800 |
|  2 | b    |  1200 |
+----+------+-------+

mysql> rollback;
Query OK, 0 rows affected (0.01 sec)

mysql> select * from user;
+----+------+-------+
| id | name | money |
+----+------+-------+
|  1 | a    |   900 |
|  2 | b    |  1100 |
+----+------+-------+

commit；
-- commit；提交后不能再撤销





-- 事务的四大特征
A 原子性：事务是最小的单位，不可以分割
C 一致性：事务要求，同一事务中的SQL语句，必须保证同时成功或同时失败
I 隔离性：事务1和事务2之间具有隔离性
D 持久性：事务一旦结束（commit，rollback），就不可以返回


事务开启：
	1.修改默认提交  set autocommit=0;
	2.begin;
	3.start transaction;
事务提交：
	commit；
事务手动回滚：
	rollback；
	

-- 事务的隔离性

1.read uncommitted；	读未提交的
2.read committed;		读已经提交的
3.repeatable read;		可以重复读
4.serializable;			串行化

----------------------------------------------------------------------
1.read uncommitted
如果有事务a 和事务b
a事务对数据进行操作，在操作的过程中，事务没有被提交，但是b可以看到a操作的结果。

bank数据库 user表
insert into user values(3,'小明',1000);
insert into user values(4,'淘宝店',1000);

-- 如何查看数据库的隔离级别
mysql 8.0
-- 系统级别
select @@global.transaction_isolation;
-- 会话级别
select @@transaction_isolation;

-- mysql的默认隔离级别  REPEATABLE-READ
mysql> select @@global.transaction_isolation;
+--------------------------------+
| @@global.transaction_isolation |
+--------------------------------+
| REPEATABLE-READ                |
+--------------------------------+

mysql> select @@transaction_isolation;
+-------------------------+
| @@transaction_isolation |
+-------------------------+
| REPEATABLE-READ         |
+-------------------------+

-- 如何修改隔离级别
set global transaction isolation level read uncommitted;

mysql> set global transaction isolation level read uncommitted;
Query OK, 0 rows affected (0.00 sec)

mysql> select @@global.transaction_isolation;
+--------------------------------+
| @@global.transaction_isolation |
+--------------------------------+
| READ-UNCOMMITTED               |
+--------------------------------+



-- 如果两个不同的地方，都在进行操作，如果事务a 开启之后，他的数据可以被其他事务读取到。
-- 这样就会出现（脏读）
-- 脏读：一个事务读到了另一个事务没有提交的数据，就叫做脏读
-- 实际开发是不允许脏读出现的


2.read committed;   读已经提交的

mysql> set global transaction isolation level read committed;
Query OK, 0 rows affected (0.00 sec)

mysql> select @@global.transaction_isolation;
+--------------------------------+
| @@global.transaction_isolation |
+--------------------------------+
| READ-COMMITTED                 |
+--------------------------------+

bank 数据库 user表

小张：银行会计
start transaction;
select * from user;
mysql> select * from user;
+----+-----------+-------+
| id | name      | money |
+----+-----------+-------+
|  1 | a         |   900 |
|  2 | b         |  1100 |
|  3 | 小明      |  1000 |
|  4 | 淘宝店    |  1000 |
+----+-----------+-------+

小张上厕所去

小王另一台终端开户
start transaction;
insert into user values(5,'小王', 100);
commit;

小张回来，计算平均值
mysql> select avg(money) from user;
+------------+
| avg(money) |
+------------+
|   820.0000 |
+------------+
--money 的平均值不是1000，变少了

-- 虽然只能读取到另外一个事务提交的数据，但是还是会出现问题，就是
-- 读取一个表的数据，发现前后不一致
-- 不可重复读现象：read committed;



3.repeatable read;  可以重复读

mysql> set global transaction isolation level repeatable read;
Query OK, 0 rows affected (0.00 sec)

mysql> select @@global.transaction_isolation;
+--------------------------------+
| @@global.transaction_isolation |
+--------------------------------+
| REPEATABLE-READ                |
+--------------------------------+

-- 在 repeatable read 隔离级别下又会出现什么问题？

-- 张全蛋-成都
start transaction;

-- 王尼玛-北京
start transaction;

-- 张全蛋-成都
mysql> insert into user values(6,'d',1000);
commit;

-- 王尼玛-北京
mysql> select * from user;
+----+-----------+-------+
| id | name      | money |
+----+-----------+-------+
|  1 | a         |   900 |
|  2 | b         |  1100 |
|  3 | 小明      |  1000 |
|  4 | 淘宝店    |  1000 |
|  5 | 小王      |   100 |
+----+-----------+-------+

mysql> insert into user values(6,'d',1000);
ERROR 1062 (23000): Duplicate entry '6' for key 'user.PRIMARY'



-- 这种现象叫做幻读
-- 事务a和事务b 同时操作一张表，事务a提交的数据，也不能被事务b读到，就可以造成幻读



4.serializable;			串行化


mysql> set global transaction isolation level serializable;
Query OK, 0 rows affected (0.00 sec)

mysql> select @@global.transaction_isolation;
+--------------------------------+
| @@global.transaction_isolation |
+--------------------------------+
| SERIALIZABLE                   |
+--------------------------------+

mysql> select * from user;
+----+-----------+-------+
| id | name      | money |
+----+-----------+-------+
|  1 | a         |   900 |
|  2 | b         |  1100 |
|  3 | 小明      |  1000 |
|  4 | 淘宝店    |  1000 |
|  5 | 小王      |   100 |
|  6 | d         |  1000 |
+----+-----------+-------+

-- 张全蛋-成都
start transaction;

-- 王尼玛-北京
start transaction;

-- 张全蛋-成都
mysql> insert into user values(7,'赵铁柱',1000);
commit;


-- 王尼玛-北京
select * from user;

-- 张全蛋-成都
begin;
mysql> insert into user values(8,'ada',1000);

---- sql 语句卡主了

-- 王尼玛-北京
commit;

-- 张全蛋-成都
Query OK, 0 rows affected (0.00 sec)


-- 当user表被另外一个事务操作的时候，其他事务里面的写操作，是不可以进行的
-- 进入排毒状态（串行化），直到王尼玛那边事务结束之后，张全蛋这个的写入操作才会执行
-- 在没有等待超时的情况下


-- 串行化问题是，性能特差！！！

READ-UNCOMMITTED > READ-COMMITTED > REPEATABLE-READ > SERIALIZABLE
-- 隔离级别越高，性能越差

mysql默认隔离级别是 REPEATABLE-READ
























