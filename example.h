class MyService : public AbstractService 
{
	
public: 
	QString contextRoot() { return "/my/service"; }	
	QStringMap& get();
	

}


QStringMap
