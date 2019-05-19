package model

import (
	"database/sql"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
	"log"
)

var MysqlDB *sql.DB


func InitMysql(){
	var err error
	MysqlDB, err = sql.Open("mysql", "tonggege:127044@tcp(118.89.30.165:3306)/DBEXP?charset=utf8")

	if err != nil{
		log.Fatalf("InitMysql error, err=%v",err)
	}

	MysqlDB.SetMaxOpenConns(10)
	_, err = MysqlDB.Query("select uid from user_login where uid=?",10)
	fmt.Printf(err.Error())
	return
}
