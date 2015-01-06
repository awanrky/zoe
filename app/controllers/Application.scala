package controllers

import play.api.mvc._
import anorm._ 
import play.api.db.DB
import play.api.Play.current

object Application extends Controller {

  def index = Action {
      
     
      DB.withConnection { implicit c =>
        
        val tests = SQL("select * from test ;")().map(row => row[String]("name") -> row[String]("description")).toList
    
        Ok(views.html.index(tests mkString "/"))
      }
    
      
      
  }

}