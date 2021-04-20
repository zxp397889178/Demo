// JsonValue is a rapidjson wrapper by chenxiang 20170829
#pragma once

#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace rjwrapper
{
	template <typename GenValue>
	std::basic_string<typename GenValue::Ch> ToString(const GenValue& rv)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		rv.Accept(writer);

		return std::basic_string<typename GenValue::Ch>(buffer.GetString(), buffer.GetLength());
	}
}

namespace util
{
	// JsonValue is a rapidjson wrapper by chenxiang 20170829
	/* 示例
	// 添加 key : value
	util::JsonValue root;						//util::JsonValue root("key0", "kv construct");kv构造也可以 "key0":"kv construct"
	root.AddMember("key1", "char* value");		//const char*		"key1":"char* value"
	root.AddMember("key2", string("string value")); //string	"key2":"string value"
	root.AddMember("key3", 1);					//int unsigned		"key3":1
	root.AddMember("key4", false);				//true bool			"key4":false
	root.AddMember("key5", 1.1);				//double float		"key5":1.1
	root("key6", uint64_t(1000000000000));		//int64_t uint64_t operator()和AddMember一样
	string json_str = root.ToString();			//{"key1":"char* value","key2":"string value","key3":1,"key4":false,"key5":1.1,"key6":1000000000000}

	// 添加 key : 数组 vector list deque set unordered_set
	vector<string> str_arr = { "abc", "def" };	//支持int uint64 bool float double string JsonValue
	util::JsonValue root_arr("arr_key", util::JsonValue(str_arr));	//{"arr_key":["abc", "def"]}
	string array_str = root_arr.ToString();		//{"arr_key":["abc", "def"]}

	// util::JsonValue数组
	vector<util::JsonValue> jv_array;
	jv_array.push_back(util::JsonValue("key0", "value0"));
	jv_array.push_back(util::JsonValue("key1", "value1"));
	util::JsonValue array_jv(jv_array);			//"[{"key0":"value0"},{"key1":"value1"}]"
	string array_jv_str = array_jv.ToString();	//[{"key0":"value0"},{"key1":"value1"}]

	// 复制一个rapidjson::Value
	Document other_doc;
	other_doc.Parse(array_jv.ToString());
	util::JsonValue cp;
	cp.CopyFrom(other_doc);						//复制Document或Value
	string cp_str = cp.ToString();				//[{"key0":"value0"},{"key1":"value1"}]

	// 设置单个值 没有key
	util::JsonValue v0(kNullType);				//null
	util::JsonValue v1;							//{}
	v1.SetValue("abc");							//支持int uint64 bool float double string JsonValue   //"abc"
	string v0_str = v0.ToString();				//null
	string v1_str = v1.ToString();				//"abc"*/

	class JsonValue
	{
	public:
		typedef rapidjson::Document				RpJsonDoc;
		typedef rapidjson::Value				RpJsonValue;
		typedef rapidjson::Value::AllocatorType RpJsonAllocator;
		typedef rapidjson::Type					RpJsonType;

		explicit JsonValue(RpJsonType type = rapidjson::kObjectType) //doc 或 value 构造
			:root_(std::make_shared<RpJsonDoc>(type)),
			alloc_(((RpJsonDoc*)root_.get())->GetAllocator())
		{
		}

		template<typename T>// int uint64 bool float double string JsonValue (key value)构造
		JsonValue(const std::string& key, const T& value)
			: JsonValue(rapidjson::kObjectType)
		{
			AddMember(key, value);
		}

		template<class TContainer>//vector list deque set unordered_set 容器构造 容器内可以是普通类型或JsonValue
		explicit JsonValue(const TContainer& container)
			: JsonValue(rapidjson::kArrayType)
		{
			Assign(container);
		}

		JsonValue(JsonValue&& other) //right value coppy construct 右值拷贝构造
			:root_(std::move(other.root_)),
			alloc_(other.alloc_)
		{
		}

		~JsonValue()
		{
		}

		template<typename T>// int uint64 bool float double string JsonValue operator()和AddMember一样
		JsonValue& operator()(const std::string& key, const T& value)
		{
			return AddMember(key, value);
		}

		template<typename T>// int uint64 bool float double string JsonValue 
		JsonValue& AddMember(const std::string& key, const T& value)
		{
			if (!set_typed_ && !root_->IsObject())
			{
				root_->SetObject();
				set_typed_ = true;
			}

			root_->AddMember(RpJsonValue(key, alloc_), to_json_value(value), alloc_);
			return *this;
		}

		template<class TContainer>//vector list deque set unordered_set  附加一个容器
		JsonValue& Assign(const TContainer& container)
		{
			if (!set_typed_ && !root_->IsArray())
			{
				root_->SetArray();
				set_typed_ = true;
			}

			for (auto& value : container)
			{
				root_->PushBack(std::move(to_json_value(value)), alloc_);
			}

			return *this;
		}

		template<typename T>// int uint64 bool float double string JsonValue 
		JsonValue& SetValue(const T& value)
		{
			return CopyFrom(to_json_value(value));
		}

		JsonValue& CopyFrom(const RpJsonValue& rp_json_value)
		{
			root_->CopyFrom(rp_json_value, alloc_);
			return *this;
		}
		
		std::string ToString()//to utf8 string
		{
			return rjwrapper::ToString(*root_);
		}

		void Clear()
		{
			root_->SetNull();
		}

		bool Empty()
		{
			return root_->IsNull();
		}

		// 想共用doc的alloc_ 可以用这个
		JsonValue CreateValue(RpJsonType type = rapidjson::kObjectType)
		{
			return std::move(JsonValue(type, alloc_));
		}

	private:
		JsonValue(const JsonValue& other) = delete;
		JsonValue& operator=(const JsonValue& other) = delete;

		JsonValue(RpJsonType type, RpJsonAllocator& _alloc)//value
			:root_(std::make_shared<RpJsonValue>(type)),
			alloc_(_alloc)
		{
		}

		template<typename T>// int uint64 bool float double
		RpJsonValue to_json_value(const T& value)
		{
			return std::move(RpJsonValue(value));
		}

		template<>// string
		RpJsonValue to_json_value(const std::string& value)
		{
			return std::move(RpJsonValue(value, alloc_));
		}

		template<>// JsonValue
		RpJsonValue to_json_value(const JsonValue& value)
		{
			//return std::move(*value.root_);
			return std::move(RpJsonValue().CopyFrom(*value.root_, alloc_));//复制一份 不销毁原来的
		}

	private:
		std::shared_ptr<RpJsonValue> root_;
		RpJsonAllocator& alloc_;
		bool set_typed_ = false;
	};
}
