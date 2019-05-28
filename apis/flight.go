package apis

import (
	"DBexp/model"
	"DBexp/utils"
	"log"
	"net/http"
)

func FlightInfo(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("apis.Login")
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
	token		:=		resq["token"]
	timeBegin	:=		resq["time_begin"]
	timeEnd		:=		resq["time_end"]
	from		:=		resq["from"]
	to			:=		resq["to"]
	if token==nil || timeBegin==nil || timeEnd==nil || from == nil || to == nil{
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}

	btime,err := utils.GetTimeByStr(timeBegin.(string))
	if err != nil{
		log.Printf("FlightInfo utils.GetTimeByStr timeBegin error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}

	etime, err := utils.GetTimeByStr(timeEnd.(string))
	if err != nil{
		log.Printf("FlightInfo utils.GetTimeByStr timeEnd error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}

	flightInfo := model.GetFlightInfoByTimeFromTo(btime ,etime, from.(string), to.(string))

	//获取site_not_ok_list
	finfol := make([]map[string]interface{},0)
	for _,f := range flightInfo{
		flMap := make(map[string]interface{})
		flMap["fid"] = f.Fid
		flMap["ftype"] = f.FType
		flMap["from"] = f.CityFrom
		flMap["to"] = f.CityTo
		flMap["begin"] = utils.GetTimeStrByTime(f.TimeBegin)
		flMap["end"] = utils.GetTimeStrByTime(f.TimeEnd)
		flMap["site_not_ok"] = model.GetSiteBookIDByFid(f.Fid)
		flMap["sitenum"] = f.SiteNum
		finfol = append(finfol,flMap)
	}

	retMap["code"] = utils.OK
	retMap["flightlist"] = finfol
	utils.SendJson(&retMap,w)
	return
}


func AddFlightInfo(w http.ResponseWriter, r *http.Request) {
	defer utils.RecoverResolve("apis.AddFlightInfo")
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

	token		:=		resq["token"]
	ftype		:=		resq["ftype"]
	from		:=		resq["from"]
	to			:=		resq["to"]
	begin		:=		resq["begin"]
	end			:=		resq["end"]
	food		:=		resq["food"]
	sitenum		:=		resq["sitenum"]
	income		:=		resq["income"]
	if token==nil || ftype==nil || from == nil || to == nil || begin==nil || end == nil||
		food == nil || sitenum==nil || income == nil{
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}

	btime,err := utils.GetTimeByStr(begin.(string))
	if err != nil{
		log.Printf("FlightInfo utils.GetTimeByStr btime error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}

	etime, err := utils.GetTimeByStr(end.(string))
	if err != nil{
		log.Printf("FlightInfo utils.GetTimeByStr etimr error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}

	fid, err := model.AddFlightInfo(utils.GenerateRandom()%100000000,ftype.(string),from.(string),to.(string),btime,etime,food.(bool), (int)(sitenum.(float64)),income.(float64))
	if err != nil{
		log.Printf("AddFlightInfo model.AddFlightInfo error, err=%v",err)
		log.Printf("fid=%v, ftype=%v, from=%v, to=%v, btime=%v, etime=%v, food=%v, sitenum=%v, income=%v",fid, ftype.(string),from.(string),to.(string),btime,etime,food.(bool),sitenum,income.(float64) )
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}

	retMap["code"] = utils.OK
	retMap["fid"] = fid
	utils.SendJson(&retMap,w)
	return
}


func ChangeFlightInfo(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("apis.ChangeFlightInfo")
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

	fid			:=		resq["fid"]
	ftype		:=		resq["ftype"]
	from		:=		resq["from"]
	to			:=		resq["to"]
	begin		:=		resq["begin"]
	end			:=		resq["end"]
	food		:=		resq["food"]
	sitenum		:=		resq["sitenum"]
	income		:=		resq["income"]
	if  ftype==nil || from == nil || to == nil || begin==nil || end == nil||
		food == nil || sitenum==nil || income == nil || fid == nil{
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}

	btime,err := utils.GetTimeByStr(begin.(string))
	if err != nil{
		log.Printf("ChangeFlightInfo utils.GetTimeByStr btime error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}

	etime, err := utils.GetTimeByStr(end.(string))
	if err != nil{
		log.Printf("ChangeFlightInfo utils.GetTimeByStr etimr error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}

	err = model.ChangeFlightInfo((int)(fid.(float64)), ftype.(string),from.(string),to.(string),btime,etime,food.(bool), (int)(sitenum.(float64)),income.(float64))
	if err != nil{
		log.Printf("ChangeFlightInfo model.AddFlightInfo error, err=%v",err)
		log.Printf("fid=%v, ftype=%v, from=%v, to=%v, btime=%v, etime=%v, food=%v, sitenum=%v, income=%v",fid, ftype.(string),from.(string),to.(string),btime,etime,food.(bool),sitenum,income.(float64) )
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}

	retMap["code"] = utils.OK
	utils.SendJson(&retMap,w)
	return

}


