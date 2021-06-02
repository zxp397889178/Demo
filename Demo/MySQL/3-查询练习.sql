-- MySQL查询练习

-- 学生表
-- Student

-- 学号
-- 姓名
-- 性别
-- 生日
-- 所在班级

create table Student (
	sno varchar(20) primary key,
	sname varchar(20) not null,
	ssex varchar(10) not null,
	sbrithday datetime,
	class varchar(20)
	);



-- 教师表
-- Teacher

-- 教师编号
-- 教师名称
-- 教师性别
-- 生日
-- 职称
-- 所在部门
create table Teacher(
	tno varchar(20) primary key,
	tname varchar(20) not null,
	tsex varchar(10) not null,
	tbrithday datetime,
	prof varchar(20),
	depart varchar(20) not null
);



-- 课程表
-- Course

-- 课程号
-- 课程名称
-- 教师编号

create table Course(
	cno varchar(20) primary key,
	cname varchar(20) not null,
	tno varchar(20) not null,
	foreign key (tno) references teacher(tno)
);



-- 成绩表
-- Sore

-- 学号
-- 课程号
-- 成绩

create table score(
	sno varchar(20) not null,
	cno varchar(20) not null,
	degree decimal,
	foreign key (sno) references student(sno),
	foreign key (cno) references course(cno),
	primary key (sno, cno)
);



------往数据表添加数据
--#添加学生信息
insert into student values('101','小红','男','1990-2-12','95033');
insert into student values('102','小黄','男','1990-3-13','95033');
insert into student values('103','小绿','女','1990-4-14','95033');
insert into student values('104','小蓝','男','1990-5-15','95033');
insert into student values('105','小黑','女','1990-6-16','95037'),('106','小白','男','1990-7-17','95038');
insert into student values('107','小紫','男','1990-8-18','95033');
insert into student values('108','小美','女','1990-9-19','95033');
insert into student values('109','小笨','男','1990-10-20','95033');


--#添加老师信息
insert into teacher values('804','李成','女','1990-2-12','教授','计算机系');
insert into teacher values('825','李二','男','1990-3-13','助理','计算机系');
insert into teacher values('831','李三','女','1990-4-14','助理','电子工程系');
insert into teacher values('856','李四','男','1990-5-15','讲师','电子工程系');


--#添加课程表
insert into course values('3-105','计算机导论','825');
insert into course values('3-245','操作系统','804');
insert into course values('6-166','数字电路','856');
insert into course values('9-888','高等数学','832');


--#添加成绩表
insert into score values('101','3-245','86');
insert into score values('102','3-105','81');
insert into score values('103','3-245','82');
insert into score values('104','3-245','83');
insert into score values('105','3-245','84');
insert into score values('106','3-245','85');
insert into score values('101','3-105','86');
insert into score values('101','6-166','87');
insert into score values('101','9-888','88');



--查询练习

-- 1.查询student表的所有记录
mysql> select * from student;
+-----+--------+------+---------------------+-------+
| sno | sname  | ssex | sbrithday           | class |
+-----+--------+------+---------------------+-------+
| 101 | 小红   | 男   | 1990-02-12 00:00:00 | 95033 |
| 102 | 小黄   | 男   | 1990-03-13 00:00:00 | 95031 |
| 103 | 小绿   | 女   | 1990-04-14 00:00:00 | 95031 |
| 104 | 小蓝   | 男   | 1990-05-15 00:00:00 | 95031 |
| 105 | 小黑   | 女   | 1990-06-16 00:00:00 | 95037 |
| 106 | 小白   | 男   | 1990-07-17 00:00:00 | 95038 |
| 107 | 小紫   | 男   | 1990-08-18 00:00:00 | 95033 |
| 108 | 小美   | 女   | 1990-09-19 00:00:00 | 95033 |
| 109 | 小笨   | 男   | 1990-10-20 00:00:00 | 95033 |
+-----+--------+------+---------------------+-------+

-- 2.查询sutdent表中的name. sex 和 class列
mysql> select sname, sno, class from student;
+--------+-----+-------+
| sname  | sno | class |
+--------+-----+-------+
| 小红   | 101 | 95033 |
| 小黄   | 102 | 95031 |
| 小绿   | 103 | 95031 |
| 小蓝   | 104 | 95031 |
| 小黑   | 105 | 95037 |
| 小白   | 106 | 95038 |
| 小紫   | 107 | 95033 |
| 小美   | 108 | 95033 |
| 小笨   | 109 | 95033 |
+--------+-----+-------+


-- 3.查询教师所有的单位，即不重复的depart列
----distinct 排除重复
mysql> select distinct depart from teacher;
+-----------------+
| depart          |
+-----------------+
| 计算机系        |
| 电子工程系      |
+-----------------+


-- 4.查询score表中成绩在60-85之间的所有记录
----查询区间 between ... and ...
mysql> select * from score where degree between 60 and 85;
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 102 | 3-105 |     81 |
| 103 | 3-245 |     82 |
| 104 | 3-245 |     83 |
| 105 | 3-245 |     84 |
| 106 | 3-245 |     85 |
+-----+-------+--------+

----直接使用运算符比较
mysql> select * from score where degree > 60 and degree < 85;
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 102 | 3-105 |     81 |
| 103 | 3-245 |     82 |
| 104 | 3-245 |     83 |
| 105 | 3-245 |     84 |
+-----+-------+--------+


-- 5.查询 score 表中成绩为85/86或88的记录
----表示或者关系的查询 in
mysql> select * from score where degree in(85,86,88);
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-105 |     86 |
| 101 | 3-245 |     86 |
| 101 | 9-888 |     88 |
| 106 | 3-245 |     85 |
+-----+-------+--------+


-- 6.查询student表中“95031”班或性别为‘女’的同学记录
---- or 表示或者
mysql> select * from student where class = '95033' or ssex = '女';
+-----+--------+------+---------------------+-------+
| sno | sname  | ssex | sbrithday           | class |
+-----+--------+------+---------------------+-------+
| 101 | 小红   | 男   | 1990-02-12 00:00:00 | 95033 |
| 103 | 小绿   | 女   | 1990-04-14 00:00:00 | 95031 |
| 105 | 小黑   | 女   | 1990-06-16 00:00:00 | 95037 |
| 107 | 小紫   | 男   | 1990-08-18 00:00:00 | 95033 |
| 108 | 小美   | 女   | 1990-09-19 00:00:00 | 95033 |
| 109 | 小笨   | 男   | 1990-10-20 00:00:00 | 95033 |
+-----+--------+------+---------------------+-------+

-- 7.以class降序的方式查询student的所有记录
---- 升序 asc(默认就是升序) 降序 desc
mysql> select * from student order by class desc;
+-----+--------+------+---------------------+-------+
| sno | sname  | ssex | sbrithday           | class |
+-----+--------+------+---------------------+-------+
| 106 | 小白   | 男   | 1990-07-17 00:00:00 | 95038 |
| 105 | 小黑   | 女   | 1990-06-16 00:00:00 | 95037 |
| 101 | 小红   | 男   | 1990-02-12 00:00:00 | 95033 |
| 107 | 小紫   | 男   | 1990-08-18 00:00:00 | 95033 |
| 108 | 小美   | 女   | 1990-09-19 00:00:00 | 95033 |
| 109 | 小笨   | 男   | 1990-10-20 00:00:00 | 95033 |
| 102 | 小黄   | 男   | 1990-03-13 00:00:00 | 95031 |
| 103 | 小绿   | 女   | 1990-04-14 00:00:00 | 95031 |
| 104 | 小蓝   | 男   | 1990-05-15 00:00:00 | 95031 |
+-----+--------+------+---------------------+-------+

---- 升序 asc 默认就是升序
mysql> select * from student order by class;


-- 8.以cno升序、degree降序查询score表的所有记录
mysql> select * from score order by cno,degree desc;
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-105 |     86 |
| 102 | 3-105 |     81 |
| 101 | 3-245 |     86 |
| 106 | 3-245 |     85 |
| 105 | 3-245 |     84 |
| 104 | 3-245 |     83 |
| 103 | 3-245 |     82 |
| 101 | 6-166 |     87 |
| 101 | 9-888 |     88 |
+-----+-------+--------+

-- 9.查询‘95033’班级的人数
---- 统计 count
mysql> select count(*) from student where class='95033';
+----------+
| count(*) |
+----------+
|        4 |
+----------+


-- 10.查询scroc 表中的最高分的学生学号和课程号（子查询或者排序）
mysql> select sno,cno from score where degree=(select max(degree) from score);
+-----+-------+
| sno | cno   |
+-----+-------+
| 101 | 9-888 |
+-----+-------+

--找到最高分
mysql> select max(degree) from score;
--找最高分的sno 和 cno
mysql> select sno,cno from score where degree=(select max(degree) from score);

-- limit 第一个数字表示从多少开始，第二个数字表示查多少条
mysql> select sno, cno,degree from score order by degree desc limit 0,1;


-- 11.查询每门课的平均成绩
---- avg()  计算平均值
mysql> select avg(degree) from score where cno='3-105';
+-------------+
| avg(degree) |
+-------------+
|     83.5000 |
+-------------+

-- group by 分组，以cno分组计算平均值
mysql> select cno,avg(degree) from score group by cno;
+-------+-------------+
| cno   | avg(degree) |
+-------+-------------+
| 3-105 |     83.5000 |
| 3-245 |     84.0000 |
| 6-166 |     87.0000 |
| 9-888 |     88.0000 |
+-------+-------------+


-- 12. 查询score表中至少有2名学生选修的，并以3开头的课程的平均分数
---- like 模糊查询，%表示任意数据  3%表示3开头的任意数据
mysql> select cno,avg(degree),count(*) from score group by cno 
having count(cno)>=1 and cno like '3%';
+-------+-------------+----------+
| cno   | avg(degree) | count(*) |
+-------+-------------+----------+
| 3-105 |     83.5000 |        2 |
| 3-245 |     84.0000 |        5 |
+-------+-------------+----------+


-- 13.查询分数大于70，小于90的sno列
mysql> select sno,degree from score where degree between 70 and 90;
+-----+--------+
| sno | degree |
+-----+--------+
| 101 |     86 |
| 101 |     86 |
| 101 |     87 |
| 101 |     88 |
| 102 |     81 |
| 103 |     82 |
| 104 |     83 |
| 105 |     84 |
| 106 |     85 |
+-----+--------+


----  多表查询
-- 14.查询所有学生的sname、cno 和degree列

mysql> select sno,sname from student;
+-----+--------+
| sno | sname  |
+-----+--------+
| 101 | 小红   |
| 102 | 小黄   |
| 103 | 小绿   |
| 104 | 小蓝   |
| 105 | 小黑   |
| 106 | 小白   |
| 107 | 小紫   |
| 108 | 小美   |
| 109 | 小笨   |
+-----+--------+

mysql> select sno,cno,degree from score;
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-105 |     86 |
| 101 | 3-245 |     86 |
| 101 | 6-166 |     87 |
| 101 | 9-888 |     88 |
| 102 | 3-105 |     81 |
| 103 | 3-245 |     82 |
| 104 | 3-245 |     83 |
| 105 | 3-245 |     84 |
| 106 | 3-245 |     85 |
+-----+-------+--------+

mysql> select sname,cno,degree from student,score where student.sno=score.sno;
+--------+-------+--------+
| sname  | cno   | degree |
+--------+-------+--------+
| 小红   | 3-105 |     86 |
| 小红   | 3-245 |     86 |
| 小红   | 6-166 |     87 |
| 小红   | 9-888 |     88 |
| 小黄   | 3-105 |     81 |
| 小绿   | 3-245 |     82 |
| 小蓝   | 3-245 |     83 |
| 小黑   | 3-245 |     84 |
| 小白   | 3-245 |     85 |
+--------+-------+--------+


-- 15.查询所有学生的sno、cname 和degree列

mysql> select sno,cname,degree from score,course where score.cno=course.cno;
+-----+-----------------+--------+
| sno | cname           | degree |
+-----+-----------------+--------+
| 101 | 计算机导论      |     86 |
| 102 | 计算机导论      |     81 |
| 101 | 操作系统        |     86 |
| 103 | 操作系统        |     82 |
| 104 | 操作系统        |     83 |
| 105 | 操作系统        |     84 |
| 106 | 操作系统        |     85 |
| 101 | 数字电路        |     87 |
| 101 | 高等数学        |     88 |
+-----+-----------------+--------+

----  三表联合查询
-- 16.查询所有学生的sname、cname 和 degree列

--sname -> student
--cname -> course;
--degree -> score;

mysql> select sname,cname,degree from student,course,score where student.sno=score.sno and course.cno=score.cno;
+--------+-----------------+--------+
| sname  | cname           | degree |
+--------+-----------------+--------+
| 小红   | 计算机导论      |     86 |
| 小黄   | 计算机导论      |     81 |
| 小红   | 操作系统        |     86 |
| 小绿   | 操作系统        |     82 |
| 小蓝   | 操作系统        |     83 |
| 小黑   | 操作系统        |     84 |
| 小白   | 操作系统        |     85 |
| 小红   | 数字电路        |     87 |
| 小红   | 高等数学        |     88 |
+--------+-----------------+--------+

mysql> select student.sno,course.cno,sname,cname,degree from score,course,student where score.cno=course.cno and score.sno=student.sno;
+-----+-------+--------+-----------------+--------+
| sno | cno   | sname  | cname           | degree |
+-----+-------+--------+-----------------+--------+
| 101 | 3-105 | 小红   | 计算机导论      |     86 |
| 102 | 3-105 | 小黄   | 计算机导论      |     81 |
| 101 | 3-245 | 小红   | 操作系统        |     86 |
| 103 | 3-245 | 小绿   | 操作系统        |     82 |
| 104 | 3-245 | 小蓝   | 操作系统        |     83 |
| 105 | 3-245 | 小黑   | 操作系统        |     84 |
| 106 | 3-245 | 小白   | 操作系统        |     85 |
| 101 | 6-166 | 小红   | 数字电路        |     87 |
| 101 | 9-888 | 小红   | 高等数学        |     88 |
+-----+-------+--------+-----------------+--------+

-- as 取别名
mysql> select student.sno as stu_sno,course.cno as cou_cno,sname,cname,degree from score,course,student where score.cno=course.cno and score.sno=student.sno;
+---------+---------+--------+-----------------+--------+
| stu_sno | cou_cno | sname  | cname           | degree |
+---------+---------+--------+-----------------+--------+
| 101     | 3-105   | 小红   | 计算机导论      |     86 |
| 102     | 3-105   | 小黄   | 计算机导论      |     81 |
| 101     | 3-245   | 小红   | 操作系统        |     86 |
| 103     | 3-245   | 小绿   | 操作系统        |     82 |
| 104     | 3-245   | 小蓝   | 操作系统        |     83 |
| 105     | 3-245   | 小黑   | 操作系统        |     84 |
| 106     | 3-245   | 小白   | 操作系统        |     85 |
| 101     | 6-166   | 小红   | 数字电路        |     87 |
| 101     | 9-888   | 小红   | 高等数学        |     88 |
+---------+---------+--------+-----------------+--------+


-- 17.查询‘95031’班学生每门课的平均分
mysql> select * from score where sno in(select sno from student where class='95031');
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 102 | 3-105 |     81 |
| 103 | 3-245 |     82 |
| 104 | 3-245 |     83 |
+-----+-------+--------+

mysql> select cno,avg(degree) from score where sno in(select sno from student where class='95031') group by cno;
+-------+-------------+
| cno   | avg(degree) |
+-------+-------------+
| 3-105 |     81.0000 |
| 3-245 |     82.5000 |
+-------+-------------+

-- 18.查询选修’3-245‘课程的成绩高于103号同学’3-245‘成绩的所有同学的记录
mysql> select degree from score where sno='103' and cno='3-245';
+--------+
| degree |
+--------+
|     82 |
+--------+

mysql> select * from score where cno='3-245' and degree > (select degree from score where sno='103' and cno='3-245');
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-245 |     86 |
| 104 | 3-245 |     83 |
| 105 | 3-245 |     84 |
| 106 | 3-245 |     85 |
+-----+-------+--------+

-- 19.查询成绩高于学号为103、课程号为“3-245”的成绩的所有记录
mysql> select * from score where degree > (select degree from score where sno='103' and cno='3-245');
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-105 |     86 |
| 101 | 3-245 |     86 |
| 101 | 6-166 |     87 |
| 101 | 9-888 |     88 |
| 104 | 3-245 |     83 |
| 105 | 3-245 |     84 |
| 106 | 3-245 |     85 |
+-----+-------+--------+

-- 20.查询和学号为108.101的同学童年出生的所有学生的sno、sname和sbirthday列

mysql> select * from student where sno in(108,101);
+-----+--------+------+---------------------+-------+
| sno | sname  | ssex | sbrithday           | class |
+-----+--------+------+---------------------+-------+
| 101 | 小红   | 男   | 1990-02-12 00:00:00 | 95033 |
| 108 | 小美   | 女   | 1990-09-19 00:00:00 | 95033 |
+-----+--------+------+---------------------+-------+

mysql> select year(sbrithday) from student where sno in(108,101);
+-----------------+
| year(sbrithday) |
+-----------------+
|            1990 |
|            1990 |
+-----------------+

mysql> select * from student where year(sbrithday) in (select year(sbrithday) from student where sno in(101,108));
+-----+--------+------+---------------------+-------+
| sno | sname  | ssex | sbrithday           | class |
+-----+--------+------+---------------------+-------+
| 101 | 小红   | 男   | 1990-02-12 00:00:00 | 95033 |
| 102 | 小黄   | 男   | 1990-03-13 00:00:00 | 95031 |
| 103 | 小绿   | 女   | 1990-04-14 00:00:00 | 95031 |
| 104 | 小蓝   | 男   | 1990-05-15 00:00:00 | 95031 |
| 105 | 小黑   | 女   | 1990-06-16 00:00:00 | 95037 |
| 106 | 小白   | 男   | 1990-07-17 00:00:00 | 95038 |
| 107 | 小紫   | 男   | 1990-08-18 00:00:00 | 95033 |
| 108 | 小美   | 女   | 1990-09-19 00:00:00 | 95033 |
| 109 | 小笨   | 男   | 1990-10-20 00:00:00 | 95033 |
+-----+--------+------+---------------------+-------+

-- 21.查询 李二 老师任课的学生成绩

mysql> select tno from teacher where tname='李二';
+-----+
| tno |
+-----+
| 825 |
+-----+

mysql> select cno from course where tno=(select tno from teacher where tname='李二');
+-------+
| cno   |
+-------+
| 3-105 |
+-------+

mysql> select * from score where cno = (select cno from course where tno=(select tno from teacher where tname='李二'));
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-105 |     86 |
| 102 | 3-105 |     81 |
+-----+-------+--------+

-- 22.查询选修某课程的同学人数多于5人的教师姓名

mysql> select cno from score group by cno having count(*)>5;
+-------+
| cno   |
+-------+
| 3-245 |
+-------+

mysql> select tno from course where cno=(select cno from score group by cno having count(*)>5);
+-----+
| tno |
+-----+
| 804 |
+-----+

mysql> select tname from teacher where tno=(select tno from course where cno=(select cno from score group by cno having count(*)>5));
+--------+
| tname  |
+--------+
| 李成   |
+--------+

mysql> select * from teacher where tno=(select tno from course where cno=(select cno from score group by cno having count(*)>5));
+-----+--------+------+---------------------+--------+--------------+
| tno | tname  | tsex | tbrithday           | prof   | depart       |
+-----+--------+------+---------------------+--------+--------------+
| 804 | 李成   | 女   | 1990-02-12 00:00:00 | 教授   | 计算机系     |
+-----+--------+------+---------------------+--------+--------------+

-- 23.查询95033班和95031班全体学生的记录
---- in 表示或者的关系
mysql> select * from student where class in ('95033','95031');
+-----+--------+------+---------------------+-------+
| sno | sname  | ssex | sbrithday           | class |
+-----+--------+------+---------------------+-------+
| 101 | 小红   | 男   | 1990-02-12 00:00:00 | 95033 |
| 102 | 小黄   | 男   | 1990-03-13 00:00:00 | 95031 |
| 103 | 小绿   | 女   | 1990-04-14 00:00:00 | 95031 |
| 104 | 小蓝   | 男   | 1990-05-15 00:00:00 | 95031 |
| 107 | 小紫   | 男   | 1990-08-18 00:00:00 | 95033 |
| 108 | 小美   | 女   | 1990-09-19 00:00:00 | 95033 |
| 109 | 小笨   | 男   | 1990-10-20 00:00:00 | 95033 |
+-----+--------+------+---------------------+-------+

-- 24.查询存在有85分以上成绩的课程cno
---- where条件查询

mysql> select cno from score where degree > 85;
+-------+
| cno   |
+-------+
| 3-105 |
| 3-245 |
| 6-166 |
| 9-888 |
+-------+

-- 25. 查询计算机系教师所教课程的成绩表

mysql> select tno from teacher where depart='计算机系';
+-----+
| tno |
+-----+
| 804 |
| 825 |
+-----+

mysql> select cno from course where tno in (select tno from teacher where depart='计算机系');
+-------+
| cno   |
+-------+
| 3-245 |
| 3-105 |
+-------+

mysql> select * from score where cno in (select cno from course where tno in (select tno from teacher where depart='计算机系'));
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-245 |     86 |
| 102 | 3-245 |     83 |
| 103 | 3-245 |     82 |
| 104 | 3-245 |     83 |
| 105 | 3-245 |     84 |
| 106 | 3-245 |     85 |
| 101 | 3-105 |     86 |
| 102 | 3-105 |     81 |
+-----+-------+--------+


-- 26.查询 计算机系 与 电子工程系 不同职称的老师的tname和prof
---- union 求并集

mysql> select * from teacher where depart='计算机系' and prof not in(select prof from teacher where depart='电子工程系');
+-----+--------+------+---------------------+--------+--------------+
| tno | tname  | tsex | tbrithday           | prof   | depart       |
+-----+--------+------+---------------------+--------+--------------+
| 804 | 李成   | 女   | 1990-02-12 00:00:00 | 教授   | 计算机系     |
+-----+--------+------+---------------------+--------+--------------+

mysql> select * from teacher where depart='电子工程系' and prof not in(select prof from teacher where depart='计算机系');
+-----+--------+------+---------------------+--------+-----------------+
| tno | tname  | tsex | tbrithday           | prof   | depart          |
+-----+--------+------+---------------------+--------+-----------------+
| 856 | 李四   | 男   | 1990-05-15 00:00:00 | 讲师   | 电子工程系      |
+-----+--------+------+---------------------+--------+-----------------+

mysql> select * from teacher where depart='计算机系' and prof not in(select prof from teacher where depart='电子工程系')
    -> union
    -> select * from teacher where depart='电子工程系' and prof not in(select prof from teacher where depart='计算机系');
+-----+--------+------+---------------------+--------+-----------------+
| tno | tname  | tsex | tbrithday           | prof   | depart          |
+-----+--------+------+---------------------+--------+-----------------+
| 804 | 李成   | 女   | 1990-02-12 00:00:00 | 教授   | 计算机系        |
| 856 | 李四   | 男   | 1990-05-15 00:00:00 | 讲师   | 电子工程系      |
+-----+--------+------+---------------------+--------+-----------------+


-- 27.查询选修编号为 3-105 课程且成绩至少高于选修编号为 3-245的同学的cno、sno、和degree，
--    并按degree从高到低排序

-- 至少  大于其中至少一个
mysql> select * from score where cno = '3-105' and degree > any(select degree from score where cno='3-245');
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-105 |     86 |
+-----+-------+--------+

mysql> select * from score where cno = '3-105' and degree > any(select degree from score where cno='3-245') order by degree desc;
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-105 |     86 |
+-----+-------+--------+

mysql> select * from score where cno = '3-245' and degree > any(select degree from score where cno='3-105') order by degree desc;
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-245 |     86 |
| 106 | 3-245 |     85 |
| 105 | 3-245 |     84 |
| 102 | 3-245 |     83 |
| 104 | 3-245 |     83 |
| 103 | 3-245 |     82 |
+-----+-------+--------+

-- 28.查询选修编号为'3-105'且成绩高于选修编号’3-245‘课程的同学的cno.sno 和degree
---- all 表示所有关系

mysql> select * from score where cno = '3-105' and degree > all(select degree from score where cno='3-245');
Empty set (0.00 sec)

-- 29.查询所有教师和同学的name，sex和birthday
---- 别名  as

mysql> select tname as name,tsex as sex,tbrithday as birthday from teacher
    -> union
    -> select sname,ssex,sbrithday from student;
+--------+-----+---------------------+
| name   | sex | birthday            |
+--------+-----+---------------------+
| 李成   | 女  | 1990-02-12 00:00:00 |
| 李二   | 男  | 1990-03-13 00:00:00 |
| 李三   | 女  | 1990-04-14 00:00:00 |
| 李四   | 男  | 1990-05-15 00:00:00 |
| 小红   | 男  | 1990-02-12 00:00:00 |
| 小黄   | 男  | 1990-03-13 00:00:00 |
| 小绿   | 女  | 1990-04-14 00:00:00 |
| 小蓝   | 男  | 1990-05-15 00:00:00 |
| 小黑   | 女  | 1990-06-16 00:00:00 |
| 小白   | 男  | 1990-07-17 00:00:00 |
| 小紫   | 男  | 1990-08-18 00:00:00 |
| 小美   | 女  | 1990-09-19 00:00:00 |
| 小笨   | 男  | 1990-10-20 00:00:00 |
+--------+-----+---------------------+

-- 30.查询所有女教师和女同学的name、sex和birthday

mysql> select tname as name,tsex as sex,tbrithday as birthday from teacher where tsex='女'
    -> union
    -> select sname,ssex,sbrithday from student where ssex='女';
+--------+-----+---------------------+
| name   | sex | birthday            |
+--------+-----+---------------------+
| 李成   | 女  | 1990-02-12 00:00:00 |
| 李三   | 女  | 1990-04-14 00:00:00 |
| 小绿   | 女  | 1990-04-14 00:00:00 |
| 小黑   | 女  | 1990-06-16 00:00:00 |
| 小美   | 女  | 1990-09-19 00:00:00 |
+--------+-----+---------------------+

-- 31.查询成绩比该课程平均成绩低的同学的成绩表

mysql> select cno,avg(degree) from score group by cno;
+-------+-------------+
| cno   | avg(degree) |
+-------+-------------+
| 3-105 |     83.5000 |
| 3-245 |     83.8333 |
| 6-166 |     87.0000 |
| 9-888 |     88.0000 |
+-------+-------------+

mysql> select * from score a where degree < (select avg(degree) from score b where a.cno=b.cno);
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 102 | 3-105 |     81 |
| 102 | 3-245 |     83 |
| 103 | 3-245 |     82 |
| 104 | 3-245 |     83 |
+-----+-------+--------+

mysql> select * from score as a where degree < (select avg(degree) from score as b where a.cno=b.cno);
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 102 | 3-105 |     81 |
| 102 | 3-245 |     83 |
| 103 | 3-245 |     82 |
| 104 | 3-245 |     83 |
+-----+-------+--------+

-- 32.查询所有任课老师的tname和depart
---- 在课程表里有安排课程
mysql> select tname,depart from teacher where tno in(select tno from course);
+--------+-----------------+
| tname  | depart          |
+--------+-----------------+
| 李成   | 计算机系        |
| 李二   | 计算机系        |
| 李三   | 电子工程系      |
| 李四   | 电子工程系      |
+--------+-----------------+

-- 33.查询至少有两名男生的班号

mysql> select class from student where ssex='男' group by class having count(*) > 1;
+-------+
| class |
+-------+
| 95033 |
| 95031 |
+-------+


-- 34.查询student中不姓 小 的同学记录
---- 模糊查询，%表示任意数据  not like 不像，like 像
mysql> select * from student where sname not like '小%';
Empty set (0.00 sec)

mysql> select * from student where sname like '小%';
+-----+--------+------+---------------------+-------+
| sno | sname  | ssex | sbrithday           | class |
+-----+--------+------+---------------------+-------+
| 101 | 小红   | 男   | 1990-02-12 00:00:00 | 95033 |
| 102 | 小黄   | 男   | 1990-03-13 00:00:00 | 95031 |
| 103 | 小绿   | 女   | 1990-04-14 00:00:00 | 95031 |
| 104 | 小蓝   | 男   | 1990-05-15 00:00:00 | 95031 |
| 105 | 小黑   | 女   | 1990-06-16 00:00:00 | 95037 |
| 106 | 小白   | 男   | 1990-07-17 00:00:00 | 95038 |
| 107 | 小紫   | 男   | 1990-08-18 00:00:00 | 95033 |
| 108 | 小美   | 女   | 1990-09-19 00:00:00 | 95033 |
| 109 | 小笨   | 男   | 1990-10-20 00:00:00 | 95033 |
+-----+--------+------+---------------------+-------+


-- 35.查询student表每个学生的姓名和年龄

---- 当前年份减出生年份

mysql> select year(now());
+-------------+
| year(now()) |
+-------------+
|        2021 |
+-------------+

mysql> select now();
+---------------------+
| now()               |
+---------------------+
| 2021-05-26 08:07:44 |
+---------------------+

mysql> select year(sbrithday) from student;
+-----------------+
| year(sbrithday) |
+-----------------+
|            1990 |
|            1990 |
|            1990 |
|            1990 |
|            1990 |
|            1990 |
|            1990 |
|            1990 |
|            1990 |
+-----------------+

mysql> select sname,year(now())-year(sbrithday) as birth from student;
+--------+-------+
| sname  | birth |
+--------+-------+
| 小红   |    31 |
| 小黄   |    31 |
| 小绿   |    31 |
| 小蓝   |    31 |
| 小黑   |    31 |
| 小白   |    31 |
| 小紫   |    31 |
| 小美   |    31 |
| 小笨   |    31 |
+--------+-------+


-- 36.查询student中最大和最小的sbrithday值

mysql> select max(sbrithday) as '最大' ,min(sbrithday) as '最小' from student;
+---------------------+---------------------+
| 最大                | 最小                |
+---------------------+---------------------+
| 1990-10-20 00:00:00 | 1990-02-12 00:00:00 |
+---------------------+---------------------+

-- 37.以班号和年龄从大到小的顺序查询student的全部记录
mysql> select * from student order by class desc,sbrithday;
+-----+--------+------+---------------------+-------+
| sno | sname  | ssex | sbrithday           | class |
+-----+--------+------+---------------------+-------+
| 106 | 小白   | 男   | 1990-07-17 00:00:00 | 95038 |
| 105 | 小黑   | 女   | 1990-06-16 00:00:00 | 95037 |
| 101 | 小红   | 男   | 1990-02-12 00:00:00 | 95033 |
| 107 | 小紫   | 男   | 1990-08-18 00:00:00 | 95033 |
| 108 | 小美   | 女   | 1990-09-19 00:00:00 | 95033 |
| 109 | 小笨   | 男   | 1990-10-20 00:00:00 | 95033 |
| 102 | 小黄   | 男   | 1990-03-13 00:00:00 | 95031 |
| 103 | 小绿   | 女   | 1990-04-14 00:00:00 | 95031 |
| 104 | 小蓝   | 男   | 1990-05-15 00:00:00 | 95031 |
+-----+--------+------+---------------------+-------+


-- 38.查询男教师及其所上的课程

mysql> select * from course where tno in ( select tno from teacher where tsex='男');
+-------+-----------------+-----+
| cno   | cname           | tno |
+-------+-----------------+-----+
| 3-105 | 计算机导论      | 825 |
| 6-166 | 数字电路        | 856 |
+-------+-----------------+-----+

-- 39.查询最高分同学的sno、cno和degree列
mysql> select * from score where degree=(select max(degree) from score);
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 9-888 |     88 |
+-----+-------+--------+

-- 40.查询与小白同性别的所有同学的sname

mysql> select sname from student where ssex=(select ssex from student where sname='小白');
+--------+
| sname  |
+--------+
| 小红   |
| 小黄   |
| 小蓝   |
| 小白   |
| 小紫   |
| 小笨   |
+--------+

-- 41.查询与小蓝同性别并且同班的所有同学的sname

mysql> select sname from student where ssex=(select ssex from student where sname='小蓝') and class = (select class from student where sname='小蓝');
+--------+
| sname  |
+--------+
| 小黄   |
| 小蓝   |
+--------+

-- 42.查询所有选修计算机导论 课程的 男 同学的成绩表

mysql> select * from score where cno=(select cno from course where cname='计算机导论') and sno in(select sno from student where ssex='男');
+-----+-------+--------+
| sno | cno   | degree |
+-----+-------+--------+
| 101 | 3-105 |     86 |
| 102 | 3-105 |     81 |
+-----+-------+--------+

-- 43.假设使用如下命令建立了一个grade表：
create table grade(
	low int(3),
	upp int(3),
	grade char(1));

insert into grade values(90,100,'A');
insert into grade values(80,89,'B');
insert into grade values(70,79,'C');
insert into grade values(60,69,'D');
insert into grade values(0,59,'E');

-- 现查询所有同学的sno、cno、 grade列

mysql> select sno,cno,grade from score,grade where degree between low and upp;
+-----+-------+-------+
| sno | cno   | grade |
+-----+-------+-------+
| 101 | 3-105 | B     |
| 101 | 3-245 | B     |
| 101 | 6-166 | B     |
| 101 | 9-888 | B     |
| 102 | 3-105 | B     |
| 102 | 3-245 | B     |
| 103 | 3-245 | B     |
| 104 | 3-245 | B     |
| 105 | 3-245 | B     |
| 106 | 3-245 | B     |
+-----+-------+-------+













