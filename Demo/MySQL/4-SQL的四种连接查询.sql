--  SQL的四种连接查询

-- 内连接

inner join 或者 join


-- 外连接

-- 1.左连接  
left join 或 left outer join

-- 2.右连接
right join 或 right outer join 

-- 3.完全外连接
full join  或 full outer join 


create database testJoin;

-- 创建两个表：
-- person 表
id,
name,
cardId

create table person(
	id int,
	name varchar(20),
	cardId int
);

-- card表
id,
name

create table card(
	id int,
	name varchar(20)
);

insert into card values(1,'饭卡'),(2,'建行卡'),(3,'农行卡'),(4,'工商卡'),(5,'邮政卡');


insert into person values(1,'张三',1);
insert into person values(2,'李四',3);
insert into person values(3,'王五',6);


-- 并没有创建外键

-- 1.inner join 查询

mysql> select * from person inner join card on person.cardId = card.id;
+------+--------+--------+------+-----------+
| id   | name   | cardId | id   | name      |
+------+--------+--------+------+-----------+
|    1 | 张三   |      1 |    1 | 饭卡      |
|    2 | 李四   |      3 |    3 | 农行卡    |
+------+--------+--------+------+-----------+

mysql> select * from person join card on person.cardId = card.id;

-- 内联查询，其实就是两张表中的数据，通过某个字段相等，查询出相关记录数据



-- 2.left join 左外连接

mysql> select * from person left join card on person.cardId=card.id;
+------+--------+--------+------+-----------+
| id   | name   | cardId | id   | name      |
+------+--------+--------+------+-----------+
|    1 | 张三   |      1 |    1 | 饭卡      |
|    2 | 李四   |      3 |    3 | 农行卡    |
|    3 | 王五   |      6 | NULL | NULL      |
+------+--------+--------+------+-----------+

mysql> select * from person left outer join card on person.cardId=card.id;
+------+--------+--------+------+-----------+
| id   | name   | cardId | id   | name      |
+------+--------+--------+------+-----------+
|    1 | 张三   |      1 |    1 | 饭卡      |
|    2 | 李四   |      3 |    3 | 农行卡    |
|    3 | 王五   |      6 | NULL | NULL      |
+------+--------+--------+------+-----------+

----  左外连接，会把左边表的所有数据取出来，而右边表的数据，如果有相等，就显示出来，如果没有，就会补NULL


-- 3.right join 右外连接

mysql> select * from person right join card on person.cardId=card.id;
+------+--------+--------+------+-----------+
| id   | name   | cardId | id   | name      |
+------+--------+--------+------+-----------+
|    1 | 张三   |      1 |    1 | 饭卡      |
| NULL | NULL   |   NULL |    2 | 建行卡    |
|    2 | 李四   |      3 |    3 | 农行卡    |
| NULL | NULL   |   NULL |    4 | 工商卡    |
| NULL | NULL   |   NULL |    5 | 邮政卡    |
+------+--------+--------+------+-----------+

mysql> select * from person right outer join card on person.cardId=card.id;
+------+--------+--------+------+-----------+
| id   | name   | cardId | id   | name      |
+------+--------+--------+------+-----------+
|    1 | 张三   |      1 |    1 | 饭卡      |
| NULL | NULL   |   NULL |    2 | 建行卡    |
|    2 | 李四   |      3 |    3 | 农行卡    |
| NULL | NULL   |   NULL |    4 | 工商卡    |
| NULL | NULL   |   NULL |    5 | 邮政卡    |
+------+--------+--------+------+-----------+

----  右外连接，会把右边表的所有数据取出来，而左边表的数据，如果有相等，就显示出来，如果没有，就会补NULL


-- 4. full join 全外连接
mysql> select * from person full join card on person.cardId=card.id;
ERROR 1054 (42S22): Unknown column 'person.cardId' in 'on clause'

---- mysql 不支持full join 
---- 使用union 合并左右连接，达到全连接效果
mysql> select * from person left outer join card on person.cardId=card.id
    -> union
    -> select * from person right outer join card on person.cardId=card.id;
+------+--------+--------+------+-----------+
| id   | name   | cardId | id   | name      |
+------+--------+--------+------+-----------+
|    1 | 张三   |      1 |    1 | 饭卡      |
|    2 | 李四   |      3 |    3 | 农行卡    |
|    3 | 王五   |      6 | NULL | NULL      |
| NULL | NULL   |   NULL |    2 | 建行卡    |
| NULL | NULL   |   NULL |    4 | 工商卡    |
| NULL | NULL   |   NULL |    5 | 邮政卡    |
+------+--------+--------+------+-----------+





