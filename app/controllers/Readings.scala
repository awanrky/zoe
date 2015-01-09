package controllers

import play.api.data.Form
import play.api.mvc._
import play.api.data.Forms._
import models._
import play.api.libs.json.Json
import play.api.libs.json.Json._


object Readings extends Controller {

  val readingForm: Form[Reading] = Form(
  mapping(
    "value" -> bigDecimal,
    "uom" -> text,
    "source" -> text,
    "sensor" -> text
  )
    ((value, uom, source, sensor) => Reading(0, value, uom, source, sensor))
    ((r: Reading) => Some(r.value, r.uom, r.source, r.sensor))
  )

  def create = Action { implicit request =>
    play.api.Logger.info(request.uri)
    play.api.Logger.info(request.body.toString)
    readingForm.bindFromRequest.fold(
      errors => {
        Ok(Json.toJson(Map("success" -> toJson(false), "id" -> toJson(0))))
      },
      reading => {
        val id = Reading.insert(reading)
        id match {
          case Some(autoIncrementId) =>
            val result = Map("success" -> toJson(true), "id" -> toJson(autoIncrementId))
            Ok(Json.toJson(result))
          case None =>
            val result = Map("success" -> toJson(false), "id" -> toJson(-1))
            Ok(Json.toJson(result))
        }
      }
    )
  }

}