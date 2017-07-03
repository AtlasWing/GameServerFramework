-- 数据库升级脚本,每次出版本时需要维护更新脚本内容
-- database.py脚本的update参数文件
#2017060101
ALTER TABLE `CHAR_BASE` ADD COLUMN `VITALITY` int(8) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `CHAR_BASE` ADD COLUMN `USE_SKILL_BINARY` blob DEFAULT NULL COMMENT '使用技能数据';
