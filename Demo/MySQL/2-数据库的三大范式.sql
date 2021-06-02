--数据库的三大范式

--1.第一范式
--1NF

--数据表中的所有字段都是不可分割的原子值
mysql> create table student(
    -> id int primary key,
    -> name varchar(20),
    -> address varchar(20)
    -> );
	
insert into student values(1,'张三','中国四川成都高新区');

--字段值还可以继续拆分的，就不满足第一范式
 create table student3 (
    id int primary key,
    name varchar(20),
    country varchar(20),
    privence varchar(30),
    city varchar(30),
   details varchar(30)
    );
	
mysql> insert into student3 value(1,'zz','中国', '福建',"福州", "晋安区");
Query OK, 1 row affected (0.01 sec)

mysql> select * from student3;
+----+------+---------+----------+--------+-----------+
| id | name | country | privence | city   | details   |
+----+------+---------+----------+--------+-----------+
|  1 | zz   | 中国    | 福建     | 福州   | 晋安区    |
+----+------+---------+----------+--------+-----------+

--范式，设计的越详细，对于某些实际操作可能更好，但是不一定都是好处


--  2.第二范式
--  必须是满足第一范式的前提下，第二范式要求，除主键外的每一列都必须完全依赖于主键。
--  如果出现不完全依赖，只可能发生在联合主键的情况下。

--订单表
create table myorder(
	product_id int,
	customer_id int,
	product_name varchar(20),
	customer_name varchar(20),
	primary key (product_id,customer_id)
	);
--问题？
-- 除主键以外的其他列，只依赖与主键的部分字段
-- 解决：拆表
create table myorder(
	order_id int primary key,
	product_id int,
	customer_id int
	);
create table product(
	id int primary key,
	name varchar(20)
	);
create table customer(
	id int primary key,
	name varchar(20)
	);
--分成3个表后，就满足了第二范式的设计！！

--3.第三范式
--必须满足第二范式，除开主键列之外，不能有传递依赖关系。
create table myorder(
	order_id int primary key,
	product_id int,
	customer_id int，
	/*customer_phone varchar(15)*/  --不能放在这里，有传递依赖，phone依赖customer_id,customer_id依赖主键
	);

create table customer(
	id int primary key,
	name varchar(20),
	phone varchar(15)
	);






























