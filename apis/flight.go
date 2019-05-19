package apis

import (
	"TicketBook/utils"
	"net/http"
)

func FlightInfo(w http.ResponseWriter, r *http.Request){
	defer utils.RecoverResolve("apis.Login")
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







}
