/* 
 * File:   json.h
 * Author: christian
 *
 * Created on 23. juli 2015, 10:32
 */

#ifndef JSON_H
#define	JSON_H

#include "heap.h"
#include <iostream>
#include <string.h>

namespace org_restfulipc 
{
	// We don't have a dedicated type for NULL, but use a null
	// string-pointer for that.

	enum class ErrorNumber
	{
		TypeMismatch,
		OutOfBounds,
		NotFound
	};

	struct JsonError
	{
		ErrorNumber errorNumber;
		char* errorMessage;
	};


	enum class JsonTypeTag
	{
		Number,
		Boolean,
		String,
		Object,
		Array
	};	

	const char* tag2str(JsonTypeTag tag);

	struct AbstractJson;

	struct AbstractJson
	{
		AbstractJson(JsonTypeTag typeTag) : typeTag(typeTag) {}
		JsonTypeTag typeTag;
	
		int drySerialize();
		int serialize(char* dest);
	};

	template<typename NodeType>
	struct GenericNode
	{
		NodeType* next;
		AbstractJson* json;
	};

	struct ArrayEntry : GenericNode<ArrayEntry>
	{
	};	
	
	struct ObjectEntry: GenericNode<ObjectEntry>	
	{
		const char* key;
	};


	template <JsonTypeTag tag>
	struct GenericJson : AbstractJson
	{
		GenericJson<tag>() : AbstractJson(tag) {}
		
		static void assertSameType(AbstractJson* other) {
			if (tag != other->typeTag) {
				char buffer[256];
				sprintf(buffer, "Type mismatch, expected: %d, got: %d\n", tag, other->typeTag);
				throw (std::string(buffer));
			}
		}
	};

	struct JsonString : GenericJson<JsonTypeTag::String>
	{
		JsonString(const char* string) : GenericJson<JsonTypeTag::String>(), string(string) {}	
		const char* string;

		int lengthNeededToSerialize()
		{
			if (!string) { // NULL
				return 4;
			}			
			else {
				int length = 0;

				for (const char* c = string; *c; c++) {
					// We escape '\'s and '"'s
					length += (*c == '\\' || *c == '"') ? 2 : 1;	
				}
				
				return length + 2; // 2 enclosing '"'s
			}
		}

		int serialize(char* dest) 
		{
			if (!string) {
				return sprintf(dest, "NULL");
			}
			else {
				int pos = 0;
				dest[pos++] = '"';
				for (const char* c = string; *c; c++) {
					if (*c == '\\' || *c == '"') {
						dest[pos++] = '\\';
					}
					dest[pos++] = *c;
				}	
				dest[pos++] = '"';
				dest[pos] = '\0';
				return pos;
			}
		}
	};


	struct JsonNumber : GenericJson<JsonTypeTag::Number>
	{
		JsonNumber(int number) : GenericJson<JsonTypeTag::Number>(), number(number) {}

		int number; // FIXME floats and such

		int lengthNeededToSerialize() 
		{
			char dummy[0];
			return snprintf(dummy, 0, "%d", number);
		}

		int serialize(char* dest) 
		{
			return sprintf(dest, "%d", number);
		}
	};

	struct JsonBoolean : GenericJson<JsonTypeTag::Boolean>
	{
		JsonBoolean(bool boolean) : GenericJson<JsonTypeTag::Boolean>(), boolean(boolean) {}

		bool boolean;

		int lengthNeededToSerialize() 
		{
			return boolean ? 4 : 5;
		}

		int serialize(char* dest) 
		{
			return sprintf(dest, "%s", boolean ? "true" : "false");
		};
	};

	struct JsonArray : GenericJson<JsonTypeTag::Array>
	{
		ArrayEntry* first;
		ArrayEntry* last;

		AbstractJson* value(uint index) 
		{
			ArrayEntry* ptr = first;
			while (ptr && index > 0) {
				ptr = ptr->next;
				index--;
			}
			
			if (!ptr) throw ErrorNumber::OutOfBounds;

			return ptr->json;
		}

		int lengthNeededToSerialize() 
		{
			int accum = 0;
			for (ArrayEntry* ptr = first; ptr; ptr = ptr->next) {
				accum += ptr->json->drySerialize();
				if (ptr != first) accum += 2;
			}

			return accum + 2;
		}

		int serialize(char* dest) 
		{
			int pos = 0;	
			dest[pos++] = '[';
			if (first) {
				pos += first->json->serialize(dest + pos);
			
				for (ArrayEntry* ptr = first->next; ptr; ptr = ptr->next) {
					dest[pos++] = ',';
					dest[pos++] = ' ';
					pos += ptr->json->serialize(dest + pos);
				}	

			}

			dest[pos++] = ']';
			dest[pos] = '\0';
			return pos;
		}
	};

	struct JsonObject : GenericJson<JsonTypeTag::Object>
	{
		ObjectEntry* first;
		ObjectEntry* last;

		AbstractJson* value(const char* key) 
		{
			for (ObjectEntry* ptr = first; ptr; ptr = ptr->next) 
				if (!strcmp(key, ptr->key)) 
					return ptr->json;

			return 0;
		}

		int lengthNeededToSerialize() {
			int accum = 0;	
			for (ObjectEntry* ptr = first; ptr; ptr = ptr->next) {
				accum += strlen(ptr->key) + 5; 
				accum += ptr->json->drySerialize();
				if (ptr != first) {
					accum += 2; // comma + space
				}
			} 

			return accum + 2;
		}

		int serialize(char* dest)
		{
			int pos = 0;
			dest[pos++] = '{';
			if (first) {
				pos += sprintf(dest + pos, "\"%s\" : ", first->key);
				pos += first->json->serialize(dest + pos);

				for (ObjectEntry* ptr = first->next; ptr; ptr = ptr->next) {
					dest[pos++] = ',';
					dest[pos++] = ' ';
					pos += sprintf(dest + pos, "\"%s\" : ", ptr->key);
					pos += ptr->json->serialize(dest + pos);
				}
			}
			dest[pos++] = '}';
			dest[pos] = '\0';
		}
	};

	template<typename JsonType>
	JsonType* jsonCast(AbstractJson* json) {
		JsonType::assertSameType(json);			
		return static_cast<JsonType*>(json);
	}

	template<typename rootJsonType>
	class JsonDoc 
	{
	public:
		JsonDoc() : pRoot(0), pCurrent(0), pHeap()
		{ 
			pCurrent = pRoot = new (pHeap.allocate<rootJsonType>()) rootJsonType();
		}

		JsonDoc(char* text) { /*FIXME*/};
		virtual ~JsonDoc() {};

		template<typename JsonType>
		JsonType* add(JsonArray* arr, const JsonType& value) {
			JsonType* valueCopy = new(pHeap.allocate<JsonType>()) JsonType(value);
			ArrayEntry* newEntry = new(pHeap.allocate<ArrayEntry>()) ArrayEntry();
			newEntry->json = valueCopy;

			if (arr->last) {
				arr->last->next = newEntry;
			}
			else {
				arr->first = newEntry;
			}

			arr->last = newEntry;
			return valueCopy;
		}

		template<typename JsonType>
		JsonType* add(JsonObject* obj, const char* key, const JsonType& value) {
			JsonType* valueCopy = new(pHeap.allocate<JsonType>()) JsonType(value);
			ObjectEntry* newEntry = new(pHeap.allocate<ObjectEntry>()) ObjectEntry();
			newEntry->key = key;
			newEntry->json = valueCopy;

			if (obj->last) {
				obj->last->next = newEntry;
			}
			else {
				obj->first = newEntry;
			}

			obj->last = newEntry;
			return valueCopy;
		}

		rootJsonType* root() { return pRoot; };		

	private:
		
		rootJsonType* pRoot;
		AbstractJson* pCurrent;	
		Heap pHeap;		
	};


	/**
	 * Specialized JsonDoc to a Hal document, meaning:
	 *	- the root element is an object
	 *  - knows about links
	 */
	class HalJsonDoc: public JsonDoc<JsonObject>
	{
	public:
		HalJsonDoc() : JsonDoc<JsonObject>() {}

		void setSelfLink(char* selfuri);
		void addRelatedLink(char* href,
							char* profile,
							char* name = 0);
		void addLink(char* relation, 
				     char* href,
					 char* profile = 0, 
					 char* name = 0);
	
		void addLink(char* relation,
					 char* href,
					 bool  templated,
					 char* type,
					 char* profile,
					 char* name,
					 char* title,
					 char* deprecation);
	};
}

/*		// Some support for HAL api 
		//href, templated, type, deprecation, name, profile, title, hreflang
*/		
	


#endif	/* JSON_H */

