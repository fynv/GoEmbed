package main

// #cgo CFLAGS: -I../GUITest
// #cgo LDFLAGS: -L. -lGUITest
// #include <stdlib.h>
// #include <memory.h>
// #include <Api.h>
// void c_btn_click_callback(void* userData);
// void c_http_get_callback(void* ptr_result, void* userData);
import "C"
import (
	"runtime"
	"runtime/cgo"
	"unsafe"
)

type CString struct {
	ptr *C.char
}

func NewCString(str string) *CString {
	pstr := &CString{C.CString(str)}
	runtime.SetFinalizer(pstr, func(pstr *CString) {
		C.free(unsafe.Pointer(pstr.ptr))
	})
	return pstr
}

func Print(str string) {
	cstr := NewCString(str)
	C.Print(cstr.ptr)
}

type HttpGetResult struct {
	ptr unsafe.Pointer
}

func NewHttpGetResult(ptr unsafe.Pointer) *HttpGetResult {
	pobj := &HttpGetResult{ptr}
	runtime.SetFinalizer(pobj, func(pobj *HttpGetResult) {
		C.HttpGetResult_Delete(pobj.ptr)
	})
	return pobj
}

func (result HttpGetResult) GetString() string {
	size := uintptr(C.HttpGetResult_GetSize(result.ptr))
	pdata := C.HttpGetResult_GetData(result.ptr)

	cstr := C.malloc(C.ulonglong(size + 1))
	C.memcpy(cstr, pdata, C.ulonglong(size))
	C.memset(unsafe.Pointer(uintptr(cstr)+size), 0, 1)
	ret := C.GoString((*C.char)(cstr))
	C.free(cstr)

	return ret
}

type HttpClient struct {
	ptr unsafe.Pointer
}

func NewHttpClient() *HttpClient {
	pobj := &HttpClient{C.HttpClient_New()}
	runtime.SetFinalizer(pobj, func(pobj *HttpClient) {
		C.HttpClient_Delete(pobj.ptr)
	})
	return pobj
}

func (client HttpClient) Get(url string) *HttpGetResult {
	c_url := NewCString(url)
	ptr := C.HttpClient_Get(client.ptr, c_url.ptr)
	return NewHttpGetResult(ptr)
}

//export c_http_get_callback
func c_http_get_callback(ptr_result unsafe.Pointer, userData unsafe.Pointer) {
	result := NewHttpGetResult(ptr_result)
	h := cgo.Handle(userData)
	callback := h.Value().(func(result *HttpGetResult))
	callback(result)
	h.Delete()
}

func (client HttpClient) GetAsync(url string, callback func(result *HttpGetResult)) {
	c_url := NewCString(url)
	h := cgo.NewHandle(callback)
	C.HttpClient_GetAsync(client.ptr, c_url.ptr, (C.HttpGetCallback)(C.c_http_get_callback), unsafe.Pointer(h))
}

type Element interface {
	ElemPtr() unsafe.Pointer
}

func GetName(elem Element) string {
	return C.GoString(C.Element_GetName(elem.ElemPtr()))
}

func SetName(elem Element, name string) {
	c_name := NewCString(name)
	C.Element_SetName(elem.ElemPtr(), c_name.ptr)
}

type Text struct {
	ptr unsafe.Pointer
}

func NewText(text string) *Text {
	c_text := NewCString(text)
	pobj := &Text{C.Text_New(c_text.ptr)}
	runtime.SetFinalizer(pobj, func(pobj *Text) {
		C.Element_Delete(pobj.ptr)
	})
	return pobj
}

func (text Text) ElemPtr() unsafe.Pointer {
	return text.ptr
}

type SameLine struct {
	ptr unsafe.Pointer
}

func NewSameLine() *SameLine {
	pobj := &SameLine{C.SameLine_New()}
	runtime.SetFinalizer(pobj, func(pobj *SameLine) {
		C.Element_Delete(pobj.ptr)
	})
	return pobj
}

func (sl SameLine) ElemPtr() unsafe.Pointer {
	return sl.ptr
}

type InputText struct {
	ptr unsafe.Pointer
}

func NewInputText(name string, size int, str string) *InputText {
	c_name := NewCString(name)
	c_str := NewCString(str)
	pobj := &InputText{C.InputText_New(c_name.ptr, C.int(size), c_str.ptr)}
	runtime.SetFinalizer(pobj, func(pobj *InputText) {
		C.Element_Delete(pobj.ptr)
	})
	return pobj
}

func (input_text InputText) ElemPtr() unsafe.Pointer {
	return input_text.ptr
}

func (input_text InputText) Text() string {
	return C.GoString(C.InputText_GetText(input_text.ptr))
}

func (input_text InputText) SetText(text string) {
	c_text := NewCString(text)
	C.InputText_SetText(input_text.ptr, c_text.ptr)
}

type Button struct {
	ptr        unsafe.Pointer
	h_on_click cgo.Handle
}

func NewButton(name string) *Button {
	c_name := NewCString(name)
	pobj := &Button{C.Button_New(c_name.ptr), 0}
	runtime.SetFinalizer(pobj, func(pobj *Button) {
		if pobj.h_on_click != 0 {
			pobj.h_on_click.Delete()
		}
		C.Element_Delete(pobj.ptr)
	})
	return pobj
}

func (button Button) ElemPtr() unsafe.Pointer {
	return button.ptr
}

//export c_btn_click_callback
func c_btn_click_callback(userData unsafe.Pointer) {
	h := cgo.Handle(userData)
	callback := h.Value().(func())
	callback()
}

func (button Button) SetOnClick(callback func()) {
	if button.h_on_click != 0 {
		button.h_on_click.Delete()
	}
	button.h_on_click = cgo.NewHandle(callback)
	C.Button_SetOnClick(button.ptr, (C.ClickCallback)(C.c_btn_click_callback), unsafe.Pointer(button.h_on_click))
}

type ScriptWindow struct {
	ptr   unsafe.Pointer
	elems []Element
}

func NewScriptWindow() *ScriptWindow {
	pobj := &ScriptWindow{C.ScriptWindow_New(), nil}
	runtime.SetFinalizer(pobj, func(pobj *ScriptWindow) {
		C.ScriptWindow_Delete(pobj.ptr)
	})
	return pobj
}

func (wnd ScriptWindow) Show() bool {
	return C.ScriptWindow_GetShow(wnd.ptr) != 0
}

func (wnd ScriptWindow) SetShow(show bool) {
	if show {
		C.ScriptWindow_SetShow(wnd.ptr, 1)
	} else {
		C.ScriptWindow_SetShow(wnd.ptr, 0)
	}
}

func (wnd ScriptWindow) Title() string {
	return C.GoString(C.ScriptWindow_GetTitle(wnd.ptr))
}

func (wnd ScriptWindow) SetTitle(title string) {
	c_title := NewCString(title)
	C.ScriptWindow_SetTitle(wnd.ptr, c_title.ptr)
}

func (wnd ScriptWindow) Add(elem Element) {
	C.ScriptWindow_Add(wnd.ptr, elem.ElemPtr())
	wnd.elems = append(wnd.elems, elem)
}

func (wnd ScriptWindow) Remove(elem Element) {
	C.ScriptWindow_Remove(wnd.ptr, elem.ElemPtr())
	for i := range wnd.elems {
		if wnd.elems[i] == elem {
			wnd.elems = append(wnd.elems[:i], wnd.elems[i+1:]...)
			break
		}
	}
}

func (wnd ScriptWindow) Clear() {
	C.ScriptWindow_Clear(wnd.ptr)
}
