package model

import (
	"DBexp/utils"
	"log"
	"time"
)

func GetBookInfo(t time.Time, fid int, uid int)(map[string]interface{},bool){
	tx := MysqlDB

	bi := make(map[string]interface{})
	rows,err := tx.Query("select bookid,book_uid,user_book.uid,time,cost,extre,siteid from user_book,flight_site where user_book.fid=flight_site.fid and user_book.uid=flight_site.uid and user_book.fid=? and (user_book.book_uid=? or user_book.uid=?)",
	fid,uid,uid)
	if err != nil{
		log.Printf("GetBookInfo tx.Query error, err=%v",err)
		return nil,false
	}
	defer rows.Close()
	for rows.Next(){
		bookid := 0
		book_uid := 0
		ubuid := 0
		_time := time.Now()
		cost := 0.0
		extre := ""
		siteid := 0
		err := rows.Scan(&bookid, &book_uid, &ubuid, &_time, &cost, &extre, &siteid)
		if err != nil{
			log.Printf("GetBookInfo rows.Scan error, err=%v",err)
			return nil,false
		}
		bi["bookid"] = bookid
		bi["book_uid"] = book_uid
		bi["uid"] = ubuid
		bi["fid"] = fid
		bi["time"] = utils.GetTimeStrByTime(_time)
		bi["income"] = cost
		bi["extra"] = extre
		bi["siteid"] = siteid
		break
	}
	return bi,true
}



