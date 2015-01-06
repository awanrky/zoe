package models

/**
 * Created by mark on 1/4/15.
 *
 */


  case class Reading(
    id: Long,
    value: BigDecimal,
    uom: String,
    source: String,
    sensor: String
  )

  object Reading {

    import anorm._
    import anorm.SqlParser._
    import play.api.db._
    import play.api.Play.current

    val reading = {
      get[Long]("id") ~
      get[java.math.BigDecimal]("value") ~
      get[String]("uom") ~
      get[String]("source") ~
      get[String]("sensor") map {
        case id~value~uom~source~sensor => Reading(id, value, uom, source, sensor)
      }
    }

    def insert(reading: Reading): Option[Long] = {
      val id: Option[Long] = DB.withConnection { implicit c =>
        SQL(
          """
            |INSERT INTO `zoe`.`reading`
            |(
            |`value`,
            |`uom_id`,
            |`created_on`,
            |`source_id`,
            |`sensor_id`)
            |SELECT {value}, uom.id, NOW(), source.id, sensor.id
            |FROM  `zoe`.`uom`
            |JOIN `zoe`.`source` ON `source`.`name` = {source}
            |JOIN `zoe`.`sensor` ON `sensor`.`name` = {sensor}
            |WHERE `uom`.`name` = {uom}
          """.stripMargin)
          .on("value" -> reading.value.bigDecimal,
              "uom" -> reading.uom,
              "source" -> reading.source,
              "sensor" -> reading.sensor
          ).executeInsert()
      }
      id
    }

    import play.api.libs.json._

  implicit object ReadingFormat extends Format[Reading] {


    def writes(reading: Reading): JsValue = {
      val readingSeq = Seq(
        "id" -> JsNumber(reading.id),
        "value" -> JsNumber(reading.value),
        "uom" -> JsString(reading.uom),
        "source" -> JsString(reading.source),
        "sensor" -> JsString(reading.sensor)
      )
      JsObject(readingSeq)
    }

    def reads(json: JsValue): JsResult[Reading] = {
      val id = (json \ "id").as[Long]
      val value = (json \ "value").as[BigDecimal]
      val uom = (json \ "uom").as[String]
      val source = (json \ "source").as[String]
      val sensor = (json \ "sensor").as[String]
      JsSuccess(Reading(id, value, uom, source, sensor))

    }
  }
}