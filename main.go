package main

import (
	"TicketBook/apis"
	"TicketBook/model"
	"log"
	"net/http"
)

func main(){

	model.InitMysql()
	http.HandleFunc("/login", apis.Login)
	http.HandleFunc("/get_flight_info",apis.FlightInfo)				//
	http.HandleFunc("/post_flight_info",apis.AddFlightInfo)
	http.HandleFunc("/put_flight_info",apis.ChangeFlightInfo)
	http.HandleFunc("/get_book_info",apis.GetBookInfo)
	http.HandleFunc("/post_book",apis.BookFlight)
	http.HandleFunc("/delete_book",apis.DeleteFlight)
	http.HandleFunc("/get_ticket_info",apis.GetTicketInfo)
	http.HandleFunc("/get_money_info",apis.GetMoneyInfo)


	err := http.ListenAndServe(":1234", nil) 					//设置监听的端口
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}

}



