/* 
 * File:   resource.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 7. marts 2015, 11:02
 */

#ifndef RESOURCE_H
#define    RESOURCE_H

#include <QReadWriteLock>
#include <QJsonObject>

class AbstractResource
{

public: 
    AbstractResource();    
    virtual ~AbstractResource() {};    

    void lockForRead();
    void lockForWrite();
    void unlock();

    char *json;
    virtual bool patch(char* jsonPatch) { return false; }

protected:
    QReadWriteLock lock;
};

class JSonObjectResource : public AbstractResource
{
public:
    JSonObjectResource();
    void update(const QJsonObject& other);

private:
    QJsonObject jsonObject;
};


#endif    /* RESOURCE_H */

