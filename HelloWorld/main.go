package main

import "C"
import (
	"runtime"
	"unsafe"
)

var window *ScriptWindow
var input_text *InputText
var http *HttpClient

func onHttpGet(result *HttpGetResult) {
	str := result.GetString()
	Print("Text from: " + input_text.Text())
	Print(str)
}

func onClick() {
	http.GetAsync(input_text.Text(), onHttpGet)
}

//export Init
func Init() {
	Print("Hello World!")

	http = NewHttpClient()

	input_text = NewInputText("##URL", 256, "https://www.gutenberg.org/cache/epub/1065/pg1065.txt")
	button_get := NewButton("Get!")
	button_get.SetOnClick(onClick)

	window = NewScriptWindow()
	window.Add(NewText("url:"))
	window.Add(NewSameLine())
	window.Add(input_text)
	window.Add(button_get)
	window.SetShow(true)
}

//export Clean
func Clean() {
	runtime.GC()
}

//export GetScriptWindow
func GetScriptWindow() unsafe.Pointer {
	return window.ptr
}

func main() {

}
