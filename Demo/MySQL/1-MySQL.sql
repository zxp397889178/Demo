-- mysql 学习笔记


/* https://blog.csdn.net/weixin_33120053/article/details/113273297
mysql>  准备好接受新命令。

-> 等待多行命令的下一行。

‘> 等待下一行，等待以单引号(“’”)开始的字符串的结束。

“> 等待下一行，等待以双引号(“””)开始的字符串的结束。

`> 等待下一行，等待以反斜点(‘`’)开始的识别符的结束。

> 等待下一行，等待以/*开始的注释的结束。
*/






-- 关系型数据库

-- 一、如何使用终端操作数据库

-- 1.如何登陆数据库
mysql> mysql -uroot -p123456

-- 2.如何查询数据库服务器中所有的数据库
mysql> show databases;
+--------------------+
| Database           |
+--------------------+
| information_schema |
| mysql              |
| performance_schema |
| sys                |
| test               |
+--------------------+
5 rows in set (0.00 sec)

-- 3.如何选择某个数据库进行操作
mysql> use test;
Database changed

-- 退出数据库
mysql> exit
Bye

-- 4.如何在数据库服务器中创建我们的数据库
mysql> create database test;
Query OK, 1 row affected (0.01 sec)

-- 创建数据表
mysql> create table pet (name varchar(20),owner varchar(20),species varchar(20),sex char(1),birth date, death date);
Query OK, 0 rows affected (0.04 sec)

-- 如何查看某个数据库中所有的数据表
mysql> show tables;
+----------------+
| Tables_in_test |
+----------------+
| pet            |
+----------------+
1 row in set (0.00 sec)

-- 查看创建好的数据库表的结构
mysql> describe pet;  -- ==desc pet;
+---------+-------------+------+-----+---------+-------+
| Field   | Type        | Null | Key | Default | Extra |
+---------+-------------+------+-----+---------+-------+
| name    | varchar(20) | YES  |     | NULL    |       |
| owner   | varchar(20) | YES  |     | NULL    |       |
| species | varchar(20) | YES  |     | NULL    |       |
| sex     | char(1)     | YES  |     | NULL    |       |
| birth   | date        | YES  |     | NULL    |       |
| death   | date        | YES  |     | NULL    |       |
+---------+-------------+------+-----+---------+-------+
6 rows in set (0.00 sec)

-- SQL 语句中的查询
mysql> select * from pet;
Empty set (0.01 sec)

-- 如何往数据表中添加数据
mysql> INSERT INTO pet VALUES('tom','cat','aa','f','1993-09-12',NULL);
Query OK, 1 row affected (0.01 sec)

--再查询一次
mysql> select * from pet;
+------+-------+---------+------+------------+-------+
| name | owner | species | sex  | birth      | death |
+------+-------+---------+------+------------+-------+
| tom  | cat   | aa      | f    | 1993-09-12 | NULL  |
+------+-------+---------+------+------------+-------+

-- MySQL常用数据类型有哪些
https://www.runoob.com/mysql/mysql-data-types.html
MySQL支持多种类型，大致可以分为三类：数值、日期/时间和字符串(字符)类型。


类型		大小	范围（有符号）	范围（无符号）	用途
TINYINT		1 byte	(-128，127)		(0，255)	小整数值
SMALLINT	2 bytes	(-32 768，32 767)	(0，65 535)	大整数值
MEDIUMINT	3 bytes	(-8 388 608，8 388 607)	(0，16 777 215)	大整数值
INT或INTEGER4 bytes	(-2 147 483 648，2 147 483 647)	(0，4 294 967 295)	大整数值
BIGINT		8 bytes	(-9,223,372,036,854,775,808，9 223 372 036 854 775 807)	(0，18 446 744 073 709 551 615)	极大整数值
FLOAT		4 bytes	(-3.402 823 466 E+38，-1.175 494 351 E-38)，0，(1.175 494 351 E-38，3.402 823 466 351 E+38)	0，(1.175 494 351 E-38，3.402 823 466 E+38)	单精度
浮点数值
DOUBLE		8 bytes	(-1.797 693 134 862 315 7 E+308，-2.225 073 858 507 201 4 E-308)，0，(2.225 073 858 507 201 4 E-308，1.797 693 134 862 315 7 E+308)	0，(2.225 073 858 507 201 4 E-308，1.797 693 134 862 315 7 E+308)	双精度
浮点数值
DECIMAL		对DECIMAL(M,D) ，如果M>D，为M+2否则为D+2	依赖于M和D的值	依赖于M和D的值	小数值



类型			大小
				( bytes)	范围	格式	用途
DATE			3			1000-01-01/9999-12-31	YYYY-MM-DD	日期值
TIME			3			'-838:59:59'/'838:59:59'	HH:MM:SS	时间值或持续时间
YEAR			1			1901/2155	YYYY	年份值
DATETIME		8			1000-01-01 00:00:00/9999-12-31 23:59:59	YYYY-MM-DD HH:MM:SS	混合日期和时间值
TIMESTAMP		4			1970-01-01 00:00:00/2038

结束时间是第 2147483647 秒，北京时间 2038-1-19 11:14:07，格林尼治时间 2038年1月19日 凌晨 03:14:07

YYYYMMDD HHMMSS	混合日期和时间值，时间戳


-- 如何删除数据
mysql> delete from pet where name='a';

-- 如何修改数据
mysql> update pet set name='ccc' where owner='aa';

-- 数据记录常用操作：
	--增加 
	INSERT INTO
	-- 删除
	DELETE
	-- 修改
	UPDATE
	-- 查找
	SELECT



--------------------------------------------------------------------------------
-- mysql建表约束

--主键约束
它能够唯一确定一张表中的一条记录，也就是我们可以通过给某个字段添加约束，就可以使得该字段不重复且不为空
mysql> create table user(
    -> id int primary key,  --primary key 主键约束
    -> name VARCHAR(20)
    -> );

----联合主键  只要联合的减值加起来不重复就可以
mysql> create table user2 (
    -> id int,
    -> name varchar(20),
    -> password varchar(20),
    -> primary key(id, name)  --由多个字段组成的主键就是联合主键，联合主键的字段都不能为空
    -> );

--自增约束
mysql> create table user3(
    -> id int primary key auto_increment,
    -> name varchar(20)
    -> );

----如果创建表时忘记主键约束：
mysql> alter table user4 add primary key (id);
----删除主键约束
mysql> alter table user4 drop primary key;
----修改主键约束  --使用modify修改字段
mysql> alter table user4 modify id int primary key;



--唯一约束
--约束修饰的字段的值不可以重复
mysql> alter table user5 add unique(name);

mysql> create table user6(
    -> id int unique,
    -> name varchar(20)
    -> );

mysql> create table user7(
    -> id int,
    -> name varchar(20),
    -> unique(id, name)   --只要id和name加起来不重复就可以
    -> );
	
---- 如何删除唯一约束
mysql> alter table user6 drop index id;
---- 添加
mysql> alter table user6 modify name varchar(20) unique;

----总结
---1.建表的时候就添加约束；
---2.可以使用alter table 。。。 add。。。。
---3.alter table ... modify....
---4.删除 alter table ... drop ....



--非空约束 not null
-- 修饰的字段不能为NULL
mysql> create table user8(
    -> id int,
    -> name varchar(20) not null
    -> );



--默认约束
-- 就是当我们插入字段值的时候，如果没有传值，就会使用默认值
mysql> create table user9(
    -> id int,
    -> name varchar(20),
    -> age int default 10
    -> );
	
----如果穿了值，就不会使用默认值
	
	

--外键约束
--涉及到两个表：父表和子表
--主表 副表
mysql> create table classes(
    -> id int primary key,
    -> name varchar(20)
    -> );

mysql> create table students(
    -> id int primary key,
    -> name varchar(20),
    -> class_id int,
    -> foreign key(class_id) references classes(id)  --class_id 必须要为classes中id有的数据
    -> );

-- 1 主表 classes 中没有的数据值，在副表中是不可以使用的
-- 2 主表中的记录被副表引用，是不可以被删除的























-- 二、如何使用可视化工具操作数据库

-- 三、如何在编程语言中操作数据库
