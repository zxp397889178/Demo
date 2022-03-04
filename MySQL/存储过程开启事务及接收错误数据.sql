CREATE DEFINER=`zhengxp`@`%` PROCEDURE `pGroup_AddVipRange`(
							IN param_isAddVip INT(10),
							IN param_groupId INT(10),
							IN param_channelGroup VARCHAR(64),
							IN param_vipRange VARCHAR(64),
							IN param_configIds VARCHAR(64)
							)
BEGIN
	#Routine body goes here...
	DECLARE var_code        INT DEFAULT 0;
	DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
	DECLARE var_vipRangeId  INT DEFAULT 0;
	DECLARE EXIT HANDLER FOR SQLEXCEPTION
	BEGIN
		ROLLBACK;
		GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
		SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
		SELECT var_code, var_message, var_vipRangeId;
	END;
	
	START TRANSACTION;
	IF (param_isAddVip = 1) THEN
		INSERT INTO `t_channelwithvip`(`channel_groupId`, `channel_group`, `vipRange`, `status`, `create_time`) VALUES 
			(param_groupId, param_channelGroup, param_vipRange, 1, NOW());
			
		SELECT LAST_INSERT_ID() INTO var_vipRangeId;
	END IF;
	
	UPDATE `t_channel` SET `channel_groupId` = param_groupId, `vipRange` = param_vipRange WHERE `configId` IN (param_configIds);
	COMMIT;
	
	SELECT var_code, var_message, var_vipRangeId;
	
END