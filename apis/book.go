package apis

import (
	"DBexp/model"
	"DBexp/utils"
	"log"
	"net/http"
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
		fidList = append(fidList,t.(int))
	}

	mmap := make([]map[string]interface{},0)
	for _,ffid := range fidList{
		bi,ok := model.GetBookInfo(time,ffid,uid)
		if ok{
			mmap = append(mmap,bi)
		}
	}

	retMap["code"] = utils.OK
	retMap["book_info_list"] = mmap
	utils.SendJson(&retMap,w)
	return

}



