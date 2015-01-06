CREATE TABLE `zoe`.`reading` (
  `id` INT NOT NULL,
  `value` DECIMAL(65,30) NOT NULL,
  `uom` VARCHAR(45) NOT NULL,
  `time` DATETIME NOT NULL,
  `source` VARCHAR(45) NOT NULL,
  `sensor` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`id`));