package main

import (
	"TicketBook/apis"
	"TicketBook/model"
	"net/http"
)

func main(){

	model.InitMysql()
	http.HandleFunc("/login", apis.Login)
	http.HandleFunc("/get_flight_info",apis.FlightInfo)				//
	http.HandleFunc("/post_flight_info",apis.AddFlightInfo)
	http.HandleFunc("/put_flight_info",apis.ChangeFlightInfo)
	http.HandleFunc("/get_book_info",apis.GetBookInfo)
	/*
	http.HandleFunc("/login",apis.WeiboCall)								//微博的回调地址
	http.HandleFunc("/login_weibo",apis.VerifyWeibo)					//设置访问的路由
	http.HandleFunc("/login_page",apis.GetVerifiedAndUserInfo)		//用户认证登录
	http.HandleFunc("/quit",apis.QuitLogin)							//用户认证退出
	http.HandleFunc("/music_recommend_video",apis.GetRecommendMusicVideo)		//用户获得推荐音乐
	http.HandleFunc("/music_recommend_up",apis.GetRecommendMusicUp)
	http.HandleFunc("/",apis.IndexPage)								//主页
*/
/*
	err := http.ListenAndServe(":10253", nil) 					//设置监听的端口
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
*/
}



