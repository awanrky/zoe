package models

/**
 * Created by mark on 1/4/15.
 *
 */


  case class Reading(
    id: Long,
    value: BigDecimal,
    uom: String,
//                      time: DateTime,
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
        SQL("insert into reading (value, uom, source, sensor, time) values ({value}, {uom}, {source}, {sensor}, NOW())")
          .on("value" -> reading.value.bigDecimal,
              "uom" -> reading.uom,
              "source" -> reading.source,
              "sensor" -> reading.sensor
          ).executeInsert()
      }
      id
    }


    import play.api.libs.json.Json
    import play.api.libs.json._

  implicit object ReadingFormat extends Format[Reading] {


    def writes(reading: Reading): JsValue = {
      val readingSeq = Seq(
        "id" -> JsNumber(reading.id),
        "value" -> JsNumber(reading.value),
        "uom" -> JsString(reading.uom),
        //      "time" -> JsString(reading.time.toString),
        "source" -> JsString(reading.source),
        "sensor" -> JsString(reading.sensor)
      )
      JsObject(readingSeq)
    }

    def reads(json: JsValue): JsResult[Reading] = {
      val id = (json \ "id").as[Long]
      val value = (json \ "value").as[BigDecimal]
      val uom = (json \ "uom").as[String]
      //      (json \ "time").as[DateTime]
      val source = (json \ "source").as[String]
      val sensor = (json \ "sensor").as[String]
      JsSuccess(Reading(id, value, uom, source, sensor))

    }
  }
}