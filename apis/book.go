package apis

import (
	"DBexp/model"
	"DBexp/utils"
	"log"
	"net/http"
	"time"
)

func GetBookInfo(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("apis.GetBookInfo")
	ok,uid,resq := TokenCheck(r)
	if !ok{
		return
	}
	retMap := make(map[string]interface{})

	if r.Method != "GET"{
		retMap["code"] = utils.METHODERR
		utils.SendJson(&retMap,w)
		return
	}

	_time		:=		resq["time"]
	_fidList	:=		resq["fid_list"]
	if _time == nil || _fidList == nil{
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}

	time ,err := utils.GetTimeByStr(_time.(string))
	if err != nil{
		log.Printf("GetBookInfo utils.GetTimeByStr btime error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}

	fidListI := _fidList.([]interface{})
	fidList := make([]int, 0)
	for _,t := range fidListI{
		fidList = append(fidList,(int)(t.(float64)))
	}

	mmap := make([]map[string]interface{},0)
	for _,ffid := range fidList{
		bi,ok := model.GetBookInfo(time,ffid,uid)
		if ok {
			mmap = append(mmap,bi)
		}
	}

	retMap["code"] = utils.OK
	retMap["book_info_list"] = mmap
	utils.SendJson(&retMap,w)
	return

}


func BookFlight(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("BookFlight")
	ok,uid,resq := TokenCheck(r)
	if !ok{
		return
	}
	retMap := make(map[string]interface{})

	if r.Method != "POST"{
		retMap["code"] = utils.METHODERR
		utils.SendJson(&retMap,w)
		return
	}

	_book_uid		:=		resq["book_uid"]
	_uid			:=		resq["uid"]
	_fid			:=		resq["fid"]
	_time			:=		resq["time"]
	_cost			:=		resq["cost"]
	_extra			:=		resq["extra"]
	_siteid			:=		resq["siteid"]

	if _fid==nil || _time==nil || _cost==nil || _siteid==nil{
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}
	bookUid := uid
	uiduid	:= uid
	if _book_uid == nil{
		bookUid = (int)(_book_uid.(float64))
	}

	if _uid != nil{
		uiduid = (int)(_uid.(float64))
	}

	time ,err := utils.GetTimeByStr(_time.(string))
	if err != nil{
		log.Printf("GetBookInfo utils.GetTimeByStr btime error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}

	fid := int(_fid.(float64))
	cost := _cost.(float64)
	extra := _extra.(string)
	siteid := (int)(_siteid.(float64))

	bookid, err := model.BookFlight(bookUid,uiduid,fid,time,cost,extra,siteid)
	if err != nil{
		log.Printf("BookFlight BookFlight error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		return
	}

	retMap["code"] = utils.OK
	retMap["bookid"] = bookid
	utils.SendJson(&retMap,w)
	return
}


func DeleteFlight(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("DeleteFlight")
	ok,_,resq := TokenCheck(r)
	if !ok{
		return
	}
	retMap := make(map[string]interface{})

	if r.Method != "POST"{
		retMap["code"] = utils.METHODERR
		utils.SendJson(&retMap,w)
		return
	}

	_bookid		:=		resq["bookid"]
	if _bookid==nil{
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}
	bookid := (int)(_bookid.(float64))
	//根据bookid获得uid\book_uid\fid\siteid
	_,uiduid,fid,siteid ,err := model.GetBookInfoByBookId(bookid)
	if err != nil{
		log.Printf("DeleteFlight GetBookInfoByBookId error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		return
	}

	err = model.DeleteUserBook(bookid,uiduid,fid,siteid)
	if err != nil{
		log.Printf("DeleteFlight DeleteUserBook error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		utils.SendJson(&retMap,w)
	}
	//check take_tick_info table and money_info whether having record

	err = model.DeleteTakeTickInfo(bookid)
	if err != nil{
		log.Printf("DeleteFlight DeleteTakeTickInfo error, err=%v",err)
	}
	moneyret,err := model.DeleteMoneyInfo(bookid)
	if err != nil{
		log.Printf("DeleteFlight DeleteMoneyInfo error, err=%v",err)
	}

	retMap["money_reply"] = moneyret
	retMap["code"] = 0
	utils.SendJson(&retMap,w)
	return
}

func GetTicketInfo(w http.ResponseWriter, r *http.Request) {
	defer utils.RecoverResolve("GetTicketInfo")
	ok,uid,resq := TokenCheck(r)
	if !ok{
		return
	}
	retMap := make(map[string]interface{})

	if r.Method != "GET"{
		retMap["code"] = utils.METHODERR
		utils.SendJson(&retMap,w)
		return
	}

	_bookid		:=		resq["bookid"]
	if _bookid==nil{
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}
	bookid := int(_bookid.(float64))

	//根据bookid获得uid\book_uid\fid\siteid
	_,uiduid,fid,siteid ,err := model.GetBookInfoByBookId(bookid)
	if err != nil{
		log.Printf("GetTicketInfo GetBookInfoByBookId error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		return
	}

	offtime,err := model.GetOffTimeByFid(fid)
	if err != nil{
		log.Printf("GetTicketInfo GetOffTimeByFid error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		return
	}

	err = model.MakeTicketInfo(bookid,uiduid,fid,time.Now(),offtime,"",0)
	if err != nil{
		log.Printf("GetTicketInfo MakeTicketInfo error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		return
	}

	ticketInfo, err := model.GetTicketInfo(bookid,fid,uid,siteid)
	if err != nil{
		log.Printf("GetTicketInfo GetTicketInfo error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		utils.SendJson(&retMap,w)
		return
	}
	ticketInfo["code"] = utils.OK
	utils.SendJson(&ticketInfo,w)
	return
}


func GetMoneyInfo(w http.ResponseWriter, r *http.Request) {
	defer utils.RecoverResolve("GetMoneyInfo")
	ok,uid,resq := TokenCheck(r)
	if !ok{
		return
	}
	retMap := make(map[string]interface{})

	if r.Method != "GET"{
		retMap["code"] = utils.METHODERR
		utils.SendJson(&retMap,w)
		return
	}

	_bookid		:=		resq["bookid"]
	_money_in	:=		resq["money_in"]
	_money_out	:= 		resq["money_out"]
	_money		:= 		resq["money"]
	if _bookid==nil || _money==nil || _money_in==nil || _money_out==nil{
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}

	bookid := int(_bookid.(float64))
	moneyIn := _money_in.(float64)
	moneyOut := _money_out.(float64)
	money := _money.(float64)

	//根据bookid获得uid\book_uid\fid\siteid
	book_uid,uiduid,fid,_ ,err := model.GetBookInfoByBookId(bookid)
	if err != nil{
		log.Printf("GetMoneyInfo GetBookInfoByBookId error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		utils.SendJson(&retMap,w)
		return
	}
	if book_uid==0 ||uiduid == 0 || fid == 0 {
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}
	err = model.MakeMoneyInfo(bookid,uid,moneyIn,moneyOut,money,time.Now())
	if err != nil{
		log.Printf("GetMoneyInfo MakeMoneyInfo error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		return
	}

	moneyInfo, err := model.GetMoneyInfo(bookid,fid)
	if err != nil{
		log.Printf("GetMoneyInfo GetMoneyInfo error, err=%v",err)
		retMap["code"] = utils.DATABASEERR
		utils.SendJson(&retMap,w)
		return
	}

	uidname,err := model.GetNameByUid(uiduid)
	if err != nil{
		uidname="nil"
	}
	moneyInfo["name"] = uidname
	bookuidName, err := model.GetNameByUid(book_uid)
	if err != nil{
		bookuidName=""
	}
	moneyInfo["book_name"] = bookuidName
	moneyInfo["code"] = utils.OK
	utils.SendJson(&moneyInfo,w)
	return
}











