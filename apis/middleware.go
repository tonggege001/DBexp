package apis

import (
	"DBexp/model"
	"DBexp/utils"
	"log"
	"net/http"
)

//检查登录的Token
func TokenCheck(r *http.Request) (bool,int,map[string]interface{}){
	retMap := make(map[string]interface{})
	resq := utils.Json2Map(r)
	if resq["token"] == nil{
		retMap["code"] = utils.NOTLOGIN
		return false, 0, nil
	}

	uid, err := model.GetUidByToken(resq["token"].(string))
	if err != nil{
		log.Printf("TokenCheck GetUidByToken err, err=%v", err)
		retMap["token"] = utils.NOTLOGIN
		return false, 0, nil
	}

	return true,uid,resq
}




