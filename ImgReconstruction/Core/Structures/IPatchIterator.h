#pragma once

class  CImage;

class IPatchIterator
{
public:
	virtual bool HasNext() = 0;
	virtual CImage GetNext() = 0;
	virtual void MoveNext() = 0;
};