class JsonPointer {
	void down(char *key);
	void down(unsigned int index);
	void up(unsigned short number = 1);

private:
	JsonPointer(JsonValue* root);
}
