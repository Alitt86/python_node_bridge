#include <node.h>
#include <Python.h>

namespace bridge {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void CallPythonFunction(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  Py_Initialize();
  PyRun_SimpleString("def add(a, b):\n"
                     "  return a + b\n");
  
  PyObject* pythonModule = PyImport_ImportModule("__main__");
  PyObject* pythonFunction = PyObject_GetAttrString(pythonModule, "add");
  
  if (pythonFunction && PyCallable_Check(pythonFunction)) {
    PyObject* arg1 = PyLong_FromLong(args[0]->NumberValue());
    PyObject* arg2 = PyLong_FromLong(args[1]->NumberValue());
    PyObject* pythonResult = PyObject_CallFunctionObjArgs(pythonFunction, arg1, arg2, NULL);
    if (pythonResult) {
      long result = PyLong_AsLong(pythonResult);
      args.GetReturnValue().Set(result);
      Py_DECREF(pythonResult);
    } else {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Error calling Python function.")));
    }
  } else {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Python function not callable.")));
  }
  
  Py_DECREF(pythonModule);
  Py_DECREF(pythonFunction);
  Py_Finalize();
}

void Initialize(Local<Object> exports) {
  NODE_SET_METHOD(exports, "callPythonFunction", CallPythonFunction);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}
