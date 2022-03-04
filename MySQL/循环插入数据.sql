/*
 Navicat Premium Data Transfer

 Source Server         : 本地虚拟机
 Source Server Type    : MySQL
 Source Server Version : 50728
 Source Host           : 192.168.87.129:3306
 Source Schema         : hrdb

 Target Server Type    : MySQL
 Target Server Version : 50728
 File Encoding         : 65001

 Date: 04/03/2022 15:28:35
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Procedure structure for pNewWorkTime_UpdateStat
-- ----------------------------
DROP PROCEDURE IF EXISTS `pNewWorkTime_UpdateStat`;
delimiter ;;
CREATE PROCEDURE `pNewWorkTime_UpdateStat`(IN param_empno CHAR(4),
					IN param_name VARCHAR(64),
					IN param_date DATE,
					IN param_setattendance INT(4))
BEGIN
	#Routine body goes here...
	
	DECLARE var_overtimesalary DECIMAL(6,2) DEFAULT 0;
	DECLARE var_leaveabsent DECIMAL(6,2) DEFAULT 0;
	DECLARE var_leaveother DECIMAL(6,2) DEFAULT 0;
	DECLARE var_elastictime INT DEFAULT 0;
	DECLARE var_leavething DECIMAL(6,2) DEFAULT 0;
	DECLARE var_leavesick DECIMAL(6,2) DEFAULT 0;
	DECLARE var_leavethingtime DECIMAL(6,2) DEFAULT 0;
	DECLARE var_month INT DEFAULT 0;
	
	if (param_setattendance = 1) then
		select SUM(`leave_absent`), SUM(`leave_others`), SUM(`elastic_time`) INTO var_leaveabsent, var_leaveother, var_elastictime from `t_work_stat` where empno=param_empno and date < DATE_FORMAT(DATE_ADD(param_date,INTERVAL 1 MONTH),'%Y-%m-%01') and date >= DATE_FORMAT(param_date,'%Y-%m-%01');
		
		select SUM(`duration`) INTO var_leavething from `tuser_document` where empno=param_empno and `status`=3 and doc_code='LEAVE_THINGS' and date < DATE_FORMAT(DATE_ADD(param_date,INTERVAL 1 MONTH),'%Y-%m-%01') and date >= DATE_FORMAT(param_date,'%Y-%m-%01');
		
		select SUM(`duration`) INTO var_leavesick from `tuser_document` where empno=param_empno and `status`=3 and doc_code='LEAVE_SICK' and date < DATE_FORMAT(DATE_ADD(param_date,INTERVAL 1 MONTH),'%Y-%m-%01') and date >= DATE_FORMAT(param_date,'%Y-%m-%01');
		
		if(var_elastictime > 3600) THEN
			set var_leavethingtime = IFNULL(var_elastictime, 0) / 3600.00;
		end if;
	end if;
	
	set var_leavethingtime = IFNULL(var_leavethingtime, 0) + IFNULL(var_leavething, 0);
	set var_leavethingtime = uptohalf(IFNULL(var_leavethingtime,0));
	set var_leavesick = uptohalf(IFNULL(var_leavesick, 0));
	set var_leaveabsent = uptohalf(IFNULL(var_leaveabsent, 0));
	set var_leaveother = uptohalf(IFNULL(var_leaveother,0));
	set var_overtimesalary = 0;
	
	set var_month = DATE_FORMAT(param_date,'%Y%m');

	if EXISTS (SELECT 1 from `t_worktime_stat` WHERE `empno` = param_empno and `salary_month`=var_month) then
		UPDATE `t_worktime_stat` SET `overtime`=var_overtimesalary, `leave_something`=var_leavethingtime, `leave_sick`=var_leavesick, `leave_absent`=var_leaveabsent, `leave_others`=var_leaveother, `name`=param_name, `work_realhours`=0 WHERE `empno` = param_empno and `salary_month`=var_month;
	else
		INSERT INTO `t_worktime_stat` (`empno`, `name`, `salary_month`, `overtime`, `leave_something`, `leave_sick`, `leave_absent`, `leave_others`, `work_realhours`) VALUES (param_empno, param_name, var_month, var_overtimesalary, var_leavethingtime, var_leavesick,var_leaveabsent, var_leaveother,  0);
	end if;
END
;;
delimiter ;

-- ----------------------------
-- Procedure structure for pSalary_WorkHourInit
-- ----------------------------
DROP PROCEDURE IF EXISTS `pSalary_WorkHourInit`;
delimiter ;;
CREATE PROCEDURE `pSalary_WorkHourInit`(IN param_year INT, IN param_month INT)
BEGIN
        DECLARE var_salary_month    INT;        -- 月份取整
        DECLARE var_salary_begin    DATE;       -- 开始日期
        DECLARE var_salary_end      DATE;       -- 结束日期
        DECLARE var_legal_workhour  INT DEFAULT 176;    -- 法定工作小时数
        
        SET var_salary_month = (param_year*10000+param_month*100)/100;
        SET var_salary_begin = CONCAT(param_year,'/',param_month,'/01');
        SET var_salary_end = DATE_SUB(DATE_ADD(var_salary_begin, INTERVAL 1 MONTH), INTERVAL 1 DAY);
        
        -- 计算本月法定的工作小时数
        SET var_legal_workhour = `getworkdays`(var_salary_begin,var_salary_end)*8;
        
        DELETE FROM `t_worktime_stat` WHERE `salary_month`=var_salary_month;
        
        -- 计算离职人员不计新小时数
        -- 需要检查入职时间是否在本月
        INSERT INTO `t_worktime_stat`(`empno`,`name`,`salary_month`,`work_hours`,`workless_time`)
        SELECT `empno`,`name`,var_salary_month,
            var_legal_workhour,
            `getworkdays`(`departure_date`, var_salary_end)*8-8
        FROM `t_employee`
        WHERE (`departure_date` IS NOT NULL) AND (`entry_date`<=var_salary_begin) 
            AND ((`departure_date`>= var_salary_begin) AND (`departure_date`<=var_salary_end));
        
        INSERT INTO `t_worktime_stat`(`empno`,`name`,`salary_month`,`work_hours`,`workless_time`)
        SELECT `empno`,`name`,var_salary_month,
            var_legal_workhour,
            (`getworkdays`(var_salary_begin, `entry_date`) +`getworkdays`(`departure_date`, var_salary_end))*8-8
        FROM `t_employee`
        WHERE (`departure_date` IS NOT NULL) AND (`entry_date`>var_salary_begin) 
            AND ((`departure_date`>= var_salary_begin) AND (`departure_date`<=var_salary_end));
        
        -- 计算在职人员工作小时数
        -- 需要检查入职时间是否在本月
        INSERT INTO `t_worktime_stat`(`empno`,`name`,`salary_month`,`work_hours`,`workless_time`)
        SELECT `empno`,`name`,var_salary_month,
            var_legal_workhour,
            0
        FROM `t_employee`
        WHERE `status`=1 AND `entry_date`<=var_salary_begin;
        
        INSERT INTO `t_worktime_stat`(`empno`,`name`,`salary_month`,`work_hours`,`workless_time`)
        SELECT `empno`,`name`,var_salary_month,
            var_legal_workhour,
            `getworkdays`(var_salary_begin, `entry_date`)*8-8
        FROM `t_employee`
        WHERE `status`=1 AND `entry_date`>var_salary_begin;
                

	END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
