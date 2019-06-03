package model

import (
	"database/sql"
	_ "github.com/go-sql-driver/mysql"
	"log"
)

var MysqlDB *sql.DB


func InitMysql(){
	var err error
	MysqlDB, err = sql.Open("mysql", "xx:xx@tcp(xxx.xxx.xxx.xxx:3306)/DBEXP?charset=utf8&parseTime=true")

	if err != nil{
		log.Fatalf("InitMysql error, err=%v",err)
	}

	MysqlDB.SetMaxOpenConns(100)
	return
}
