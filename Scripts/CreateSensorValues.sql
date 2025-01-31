--
-- Tabellenstruktur für Tabelle `SensorValues`
--
SET @storedDataBase := IFNULL(DATABASE(), 'umlaufpumpe');
USE `umlaufpumpe`;
DROP TABLE IF EXISTS `SensorValues`;
CREATE TABLE IF NOT EXISTS `SensorValues` (
  `MeasuringTime` timestamp NOT NULL DEFAULT current_timestamp(),
  `PhyName`       varchar(32) NOT NULL,
  `LogName`       varchar(128) DEFAULT NULL,
  `Type`    varchar(32) DEFAULT NULL,
  `Unit`          varchar(32) DEFAULT NULL,
  `MeasuringValue` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

DROP TABLE IF EXISTS `SensorAssigns`;
CREATE TABLE IF NOT EXISTS `SensorAssigns` (
  `PhyName`       varchar(32) NOT NULL,
  `LogName`       varchar(128) NOT NULL,
  `Type`          varchar(32) DEFAULT NULL,
  `Unit`          varchar(32) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--USE @storedDataBase;
