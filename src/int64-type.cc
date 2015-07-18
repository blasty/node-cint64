#include "int64-type.h"

#include <assert.h>

#include <node_buffer.h>

using namespace v8;

Persistent<Function> Int64Wrapper::constructor;
Persistent<Value> Int64Wrapper::prototype;

void Int64Wrapper::Init(Handle<Object> exports) {
    // FIXME: GetCurrent() may be deprecated soonish
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, Int64Wrapper::New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "Int64"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // add methods here
    NODE_SET_PROTOTYPE_METHOD(tpl, "add", Int64Wrapper::Add);
    NODE_SET_PROTOTYPE_METHOD(tpl, "sub", Int64Wrapper::Subtract);
    NODE_SET_PROTOTYPE_METHOD(tpl, "mul", Int64Wrapper::Multiply);
    NODE_SET_PROTOTYPE_METHOD(tpl, "div", Int64Wrapper::Divide);
    NODE_SET_PROTOTYPE_METHOD(tpl, "mod", Int64Wrapper::Modulo);
    NODE_SET_PROTOTYPE_METHOD(tpl, "shiftLeft", Int64Wrapper::ShiftLeft);
    NODE_SET_PROTOTYPE_METHOD(tpl, "shiftRight", Int64Wrapper::ShiftRight);
    NODE_SET_PROTOTYPE_METHOD(tpl, "xor", Int64Wrapper::Xor);
    NODE_SET_PROTOTYPE_METHOD(tpl, "or", Int64Wrapper::Or);
    NODE_SET_PROTOTYPE_METHOD(tpl, "and", Int64Wrapper::And);

    NODE_SET_PROTOTYPE_METHOD(tpl, "neg", Int64Wrapper::Negate);
    NODE_SET_PROTOTYPE_METHOD(tpl, "not", Int64Wrapper::Not);

    NODE_SET_PROTOTYPE_METHOD(tpl, "gt", Int64Wrapper::GreaterThan);
    NODE_SET_PROTOTYPE_METHOD(tpl, "geq", Int64Wrapper::GreaterOrEqual);
    NODE_SET_PROTOTYPE_METHOD(tpl, "lt", Int64Wrapper::LessThan);
    NODE_SET_PROTOTYPE_METHOD(tpl, "leq", Int64Wrapper::LessOrEqual);
    NODE_SET_PROTOTYPE_METHOD(tpl, "eq", Int64Wrapper::Equals);
    NODE_SET_PROTOTYPE_METHOD(tpl, "neq", Int64Wrapper::NotEquals);

    NODE_SET_PROTOTYPE_METHOD(tpl, "toString", Int64Wrapper::ToString);
    NODE_SET_PROTOTYPE_METHOD(tpl, "toNumber", Int64Wrapper::ToNumber);

    // set the constructor so we can use it
    constructor.Reset(isolate, tpl->GetFunction());

    // set the prototype so it can be used for typechecking
    Local<Object> obj = tpl->GetFunction()->NewInstance();
    prototype.Reset(isolate, obj->GetPrototype());

    exports->Set(String::NewFromUtf8(isolate, "Int64"), tpl->GetFunction());
}

Int64Wrapper::Int64Wrapper(int v) : val(v) {}
Int64Wrapper::~Int64Wrapper() {}

void Int64Wrapper::New(const FunctionCallbackInfo<Value> &args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (!args.IsConstructCall()) {
        isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate,
                    "Use the new operator to create instances of this object."))
            );
        return;
    }

    uint64_t val = 0;
    if (args.Length() > 0) {
        if (node::Buffer::HasInstance(args[0])) {
            auto data = node::Buffer::Data(args[0]);
            auto len = node::Buffer::Length(args[0]);
            for (size_t i = 0; i < len && i < sizeof(val); i++) {
                val = val | data[i] << (i << 3);
            }
        } else {
            val = args[0]->IsUndefined()? 0 : args[0]->NumberValue();
        }
    }

    Int64Wrapper* obj = new Int64Wrapper(val);
    obj->Wrap(args.This());

    args.GetReturnValue().Set(args.This());
}

void Int64Wrapper::Add(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val + other.val;
    }, args);
}
void Int64Wrapper::Subtract(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val - other.val;
    }, args);
}
void Int64Wrapper::Multiply(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val * other.val;
    }, args);
}
void Int64Wrapper::Divide(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val / other.val;
    }, args);
}
void Int64Wrapper::Modulo(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val % other.val;
    }, args);
}
void Int64Wrapper::ShiftLeft(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val << other.val;
    }, args);
}
void Int64Wrapper::ShiftRight(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val >> other.val;
    }, args);
}
void Int64Wrapper::Xor(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val ^ other.val;
    }, args);
}
void Int64Wrapper::Or(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val | other.val;
    }, args);
}
void Int64Wrapper::And(const FunctionCallbackInfo<Value>& args) {
    wrapBinaryOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> uint64_t {
        return self.val & other.val;
    }, args);
}

void Int64Wrapper::Not(const FunctionCallbackInfo<Value>& args) {
    wrapUnaryOp([](const Int64Wrapper& self) -> uint64_t {
        return !self.val;
    }, args);
}

void Int64Wrapper::Negate(const FunctionCallbackInfo<Value>& args) {
    wrapUnaryOp([](const Int64Wrapper& self) -> uint64_t {
        return -self.val;
    }, args);
}

void Int64Wrapper::GreaterThan(const FunctionCallbackInfo<Value>& args) {
    wrapBoolOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> bool {
            return self.val > other.val;
    }, args);
}

void Int64Wrapper::GreaterOrEqual(const FunctionCallbackInfo<Value>& args) {
    wrapBoolOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> bool {
            return self.val >= other.val;
    }, args);
}

void Int64Wrapper::LessThan(const FunctionCallbackInfo<Value>& args) {
    wrapBoolOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> bool {
            return self.val < other.val;
    }, args);
}

void Int64Wrapper::LessOrEqual(const FunctionCallbackInfo<Value>& args) {
    wrapBoolOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> bool {
            return self.val <= other.val;
    }, args);
}

void Int64Wrapper::Equals(const FunctionCallbackInfo<Value>& args) {
    wrapBoolOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> bool {
            return self.val == other.val;
    }, args);
}

void Int64Wrapper::NotEquals(const FunctionCallbackInfo<Value>& args) {
    wrapBoolOp([](const Int64Wrapper& self, const Int64Wrapper& other) -> bool {
            return self.val != other.val;
    }, args);
}

void Int64Wrapper::ToNumber(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Int64Wrapper* self = ObjectWrap::Unwrap<Int64Wrapper>(args.Holder());
    args.GetReturnValue().Set(Number::New(isolate, (double) self->val));
}

void Int64Wrapper::ToString(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    const Int64Wrapper* self = ObjectWrap::Unwrap<Int64Wrapper>(args.Holder());
    auto toChar = [](int64_t val) -> char {
        const char alphaArray[] = 
            "0123456789acdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+/";
        assert(val >= 0 && (unsigned) val < sizeof(alphaArray));
        return alphaArray[val];
    };

    int64_t tmp = self->val;
    int64_t base = 10;
    if (args.Length() > 0 && args[0]->IsNumber()) {
        base = args[0]->ToInteger()->Value();
        if (base < 0) base = -base;
        if (base > 64) base = 64;
        if (base == 0) base = 10;
    }
    // worst case is base2, in which case we'll have at most 63 digits, plus
    // one for sign and one for the null
    char res[65];
    auto idx = 0;
    // add a negative sign if it's negative
    if (tmp < 0) {
        res[0] = '-';
        tmp = -tmp;
        idx++;
    }
    // this actually puts them in reverse order; we'll unreverse in a second
    const auto start = idx;
    while (tmp > 0 && idx < 65) {
        res[idx] = toChar(tmp % base);
        tmp = tmp / base;
        idx++;
    }
    res[idx] = '\0';
    const auto len = idx - start;
    idx = 0;
    // while they aren't crossing, swap characters to unreverse the string
    while (idx < (len - idx - 1)) {
        auto tmp = res[start + idx];
        res[start + idx] = res[start+len-idx-1];
        res[start+len-idx-1] = tmp;
        idx++;
    }

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, res));
}
