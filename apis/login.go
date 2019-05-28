package apis

import (
	"DBexp/model"
	"DBexp/utils"
	"log"
	"net/http"
)


func Login(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("apis.Login")
	retMap := make(map[string]interface{})

	if r.Method != "POST"{
		log.Printf("apis.Login r.Method not match, method=%v",r.Method)
		retMap["code"] = utils.METHODERR
		utils.SendJson(&retMap,w)
		return
	}

	resq := utils.Json2Map(r)
	_uid := resq["uid"]
	if _uid == nil{
		log.Printf("apis.Login no resq value")
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}
	uid := (int)(_uid.(float64))

	login,uidType,err := model.VerifyUid(uid)
	if err != nil{
		log.Printf("apis.Login VerifyUid error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		utils.SendJson(&retMap,w)
		return
	}
	if !login{
		retMap["code"] = utils.NOTLOGIN
		utils.SendJson(&retMap,w)
		return
	}
	token, err :=model.GetTokenByUid(uid)
	if err != nil{
		log.Printf("GetTokenByUid error, err=%v",err)
		retMap["code"] = utils.PARAMERR
		return
	}
	retMap["code"] = 0
	retMap["token"] = token
	retMap["type"] = uidType
	utils.SendJson(&retMap,w)
	return

}







