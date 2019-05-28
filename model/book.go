package model

import (
	"DBexp/utils"
	"log"
	"time"
)

func GetBookInfo(t time.Time, fid int, uid int)(map[string]interface{},bool){
	tx := MysqlDB
	ok := false
	bi := make(map[string]interface{})
	rows,err := tx.Query("select bookid,book_uid,user_book.uid,time,cost,extra,siteid from user_book,flight_site where user_book.fid=flight_site.fid and user_book.uid=flight_site.uid and user_book.fid=? and (user_book.book_uid=? or user_book.uid=?) and state != ?",
	fid,uid,uid,3)
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
		ok = true
		break
	}
	return bi,ok
}

func BookFlight(bookUid int, uiduid int, fid int, t time.Time, cost float64,extra string, siteid int)(int,error){

	tx, err := MysqlDB.Begin()
	if err != nil{
		return 0,err
	}

	defer func(){
		if p:= recover();p!=nil{
			tx.Rollback()
			panic(p)
		}
	}()

	res,err := tx.Exec("insert into user_book(book_uid, uid, fid, time, cost, extra,state) VALUES (?,?,?,?,?,?,?)",
		bookUid,uiduid,fid,t,cost,extra,0)
	if err != nil{
		tx.Rollback()
		return 0,err
	}

	bookid,err := res.LastInsertId()
	if err != nil{
		tx.Rollback()
		return 0,err
	}

	res,err = tx.Exec("insert into flight_site(fid,uid,siteid) values (?,?,?)",
		fid,uiduid,siteid)
	if err != nil{
		tx.Rollback()
		return 0,err
	}
	err  = tx.Commit()
	if err != nil{
		return 0,err
	}
	return (int)(bookid),nil
}


func GetBookInfoByBookId(bookid int)(int,int,int,int,error){
	bookuid := 0
	uiduid := 0
	fid := 0
	siteid := 0

	tx := MysqlDB
	rows,err := tx.Query("select book_uid,user_book.uid,user_book.fid,siteid from user_book left join flight_site on user_book.fid=flight_site.fid and user_book.uid=flight_site.uid where bookid=?",bookid)
	if err != nil{
		log.Printf("GetBookInfo tx.Query error, err=%v",err)
		return 0,0,0,0,err
	}

	defer rows.Close()
	for rows.Next(){
		err := rows.Scan(&bookuid, &uiduid, &fid, &siteid)
		if err != nil{
			log.Printf("GetBookInfoByBookId rows.Scan error, err=%v",err)
			return 0, 0, 0, 0, err
		}
		break
	}
	return bookuid,uiduid,fid,siteid,nil
}

//删除需要事务
func DeleteUserBook(bookid int, uiduid int, fid int, siteid int) error{
	tx, err := MysqlDB.Begin()
	if err != nil{
		return err
	}

	defer func(){
		if p:= recover();p!=nil{
			tx.Rollback()
			panic(p)
		}
	}()

	_,err = tx.Exec("delete from user_book where bookid=? and state != ?",bookid,3)
	if err != nil{
		tx.Rollback()
		return err
	}
	_, err = tx.Exec("delete from flight_site where uid=? and fid=? and siteid=?",uiduid,fid,siteid)
	if err != nil{
		tx.Rollback()
		return err
	}

	err = tx.Commit()
	if err != nil{
		return err
	}
	return nil
}

func DeleteTakeTickInfo(bookid int) error {
	tx := MysqlDB
	stmt, err := tx.Prepare("delete from take_ticket_info where bookid=?")
	if err != nil{
		log.Printf("DeleteTakeTickInfo tx.Prepare error, err=%v",err)
		return err
	}

	_, err = stmt.Exec(bookid)
	if err != nil{
		log.Printf("DeleteTakeTickInfo stmt.Exec error, err=%v",err)
		return err
	}
	defer stmt.Close()

	return nil
}


func DeleteMoneyInfo(bookid int)(float64,error){
	tx := MysqlDB
	money := 0.0
	rows,err := tx.Query("select money from money_info where bookid=?",bookid)

	if err != nil{
		log.Printf("DeleteMoneyInfo tx.Query error, err=%v",err)
		return 0.0,err
	}

	for rows.Next(){
		err := rows.Scan(&money)
		if err != nil{
			return 0,err
		}
		break
	}
	rows.Close()
	if money == 0.0{
		return 0.0,nil
	}

	stmt, err := tx.Prepare("delete from money_info where bookid=?")
	if err != nil{
		log.Printf("DeleteMoneyInfo tx.Prepare error, err=%v",err)
		return 0.0, err
	}

	_, err = stmt.Exec(bookid)
	if err != nil{
		log.Printf("DeleteMoneyInfo stmt.Exec error, err=%v",err)
		return 0.0,err
	}
	stmt.Close()

	return money,nil

}

func GetOffTimeByFid(fid int)(time.Time,error){
	tx := MysqlDB
	t := time.Now()
	rows,err := tx.Query("select time_begin from flight_info where fid=?",fid)

	if err != nil{
		log.Printf("GetOffTimeByFid tx.Query error, err=%v",err)
		return t,err
	}

	for rows.Next(){
		err := rows.Scan(&t)
		if err != nil{
			return t,err
		}
		break
	}
	rows.Close()
	return t,nil
}

func MakeTicketInfo(bookid int, uid int, fid int, tnow time.Time,toff time.Time,extra string,state int)error{
	tx := MysqlDB
	id := -1
	flag := 0
	rows,err := tx.Query("select id from take_ticket_info where bookid = ?",bookid)
	if err != nil{
		return err
	}

	for rows.Next() {
		err := rows.Scan(&id)
		if err != nil{
			flag = 1
			break
		}

		if id != -1{
			flag = 1
			break
		}
	}
	rows.Close()
	//插入元素
	if flag ==0{
		stmt, err := tx.Prepare("insert into take_ticket_info(bookid,uid,fid,time,offtime,extra,state) VALUES (?,?,?,?,?,?,?)")
		if err != nil{
			log.Printf("MakeTicketInfo tx.Prepare error, err=%v",err)
			return err
		}
		_, err = stmt.Exec(bookid,uid,fid,tnow,toff,extra,state)
		if err != nil{
			return err
		}
		defer stmt.Close()
	}
	return nil
}

func GetTicketInfo(bookid int,uid int, fid int,siteid int)(map[string]interface{},error){
	mmap := make(map[string]interface{})
	tx := MysqlDB

	rows,err := tx.Query("select take_ticket_info.id,user_info.name,take_ticket_info.time,ftype,city_from,city_to,take_ticket_info.offtime,take_ticket_info.extra from take_ticket_info,user_info,flight_info where take_ticket_info.bookid=? and user_info.uid=take_ticket_info.uid and flight_info.fid=take_ticket_info.fid",bookid)
	if err != nil{
		log.Printf("GetTicketInfo tx.Query error, err=%v",err)
		return mmap, err
	}

	for rows.Next(){
		id := 0
		name := ""
		ttime := time.Now()
		ftype := ""
		cityFrom := ""
		cityTo := ""
		offtime := time.Now()
		extra := ""
		err := rows.Scan(&id, &name, &ttime, &ftype, &cityFrom, &cityTo,&offtime, &extra)
		if err != nil{
			return nil,err
		}

		mmap["id"] = id
		mmap["name"] = name
		mmap["time"] = utils.GetTimeStrByTime(ttime)
		mmap["ftype"] = ftype
		mmap["city_from"] = cityFrom
		mmap["city_to"] = cityTo
		mmap["offtime"] = utils.GetTimeStrByTime(offtime)
		mmap["extra"] = extra
		mmap["uid"] = uid
		mmap["fid"] = fid
		mmap["siteid"] = siteid
		break
	}
	defer rows.Close()
	return mmap,nil
}


func MakeMoneyInfo(bookid int, uid int, moneyIn float64, moneyOut float64, money float64,t time.Time) error{
	tx := MysqlDB
	id := -1
	flag := 0
	rows,err := tx.Query("select miid from money_info where bookid = ?",bookid)
	if err != nil{
		return err
	}

	for rows.Next() {
		err := rows.Scan(&id)
		if err != nil{
			flag = 1
			break
		}

		if id != -1{
			flag = 1
			break
		}
	}
	rows.Close()
	//插入元素
	if flag == 0{
		stmt, err := tx.Prepare("insert into money_info(bookid, uid, money_in,money_out, money,time) VALUES (?,?,?,?,?,?)")
		if err != nil{
			log.Printf("MakeMoneyInfo tx.Prepare error, err=%v",err)
			return err
		}
		_, err = stmt.Exec(bookid,uid,moneyIn,moneyOut,money,t)
		if err != nil{
			return err
		}
		defer stmt.Close()
	}
	return nil
}


func GetMoneyInfo(bookid int,fid int) (map[string]interface{},error){
	mmap := make(map[string]interface{})
	tx := MysqlDB

	rows,err := tx.Query("select money_info.miid,user_book.book_uid,user_book.uid,ftype,flight_info.time_begin,money_info.money_in,money_info.money_out,money_info.money from user_book,money_info,flight_info where user_book.bookid=? and flight_info.fid=? and money_info.bookid = ?",bookid,fid,bookid)
	if err != nil{
		log.Printf("GetMoneyInfo tx.Query error, err=%v",err)
		return mmap, err
	}

	for rows.Next(){
		miid := 0
		book_uid := 0
		uid := 0
		ftype := ""
		offtime := time.Now()
		moneyIn := 0.0
		moneyOut := 0.0
		money := 0.0
		err := rows.Scan(&miid, &book_uid, &uid, &ftype, &offtime, &moneyIn,&moneyOut, &money)
		if err != nil{
			return nil,err
		}

		mmap["miid"] = miid
		mmap["book_uid"] = uid
		mmap["uid"] = uid
		mmap["fid"] = fid
		mmap["ftype"] = ftype
		mmap["time"] = utils.GetTimeStrByTime(time.Now())
		mmap["offtime"] = utils.GetTimeStrByTime(offtime)
		mmap["money_in"] = moneyIn
		mmap["money_out"] = moneyOut
		mmap["money"] = money
		break
	}
	defer rows.Close()
	return mmap,nil
}









