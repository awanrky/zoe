package controllers

import play.api.data.Form
import play.api.libs.json.Json
import play.api._
import play.api.mvc._
import play.api.data._
import play.api.data.Forms._
import play.api.data.validation.Constraints._
import views._
import models._
import scala.collection.mutable.ArrayBuffer
import play.api.libs.json._
import play.api.libs.json.Json
import play.api.libs.json.Json._
import play.api.data.format.Formats._
import java.util.Calendar


object Readings extends Controller {

  val readingForm: Form[Reading] = Form(
  mapping(
//    "id" -> nonEmptyText,
    "value" -> bigDecimal,
    "uom" -> nonEmptyText,
//    "time" -> nonEmptyText,
    "source" -> nonEmptyText,
    "sensor" -> nonEmptyText
  )
    ((value, uom, source, sensor) => Reading(0, value, uom, source, sensor))
    ((r: Reading) => Some(r.value, r.uom, r.source, r.sensor))
  )

  def create = Action { implicit request =>
    readingForm.bindFromRequest.fold(
      errors => {
        Ok(Json.toJson(Map("success" -> toJson(false), "msg" -> toJson("Boom!"), "id" -> toJson(0))))
      },
      reading => {
        val id = Reading.insert(reading)
        id match {
          case Some(autoIncrementId) =>
            val result = Map("success" -> toJson(true), "msg" -> toJson("Success!"), "id" -> toJson(autoIncrementId))
            Ok(Json.toJson(result))
          case None =>
            val result = Map("success" -> toJson(true), "msg" -> toJson("Success!"), "id" -> toJson(-1))
            Ok(Json.toJson(result))

        }
      }
    )
  }

//  def get(id: Integer) = Action {
//
////    case class Row(name: String, description: String)
//
////    implicit val rowFormat = Json.format[Row]
//
//
//
//
//    DB.withConnection { implicit c =>
//
//      val tests = SQL("select * from reading ;")
//
////      Ok(Json.toJson(tests))
//      Ok(tests)
//    }
//  }
//
//  def create = Action {
//    Ok("test")
//  }

}