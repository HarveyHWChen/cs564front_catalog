#include "catalog.h"


RelCatalog::RelCatalog(Status &status) :
	 HeapFile(RELCATNAME, status)
{
// nothing should be needed here
}


const Status RelCatalog::getInfo(const string & relation, RelDesc &record)
{
  if (relation.empty())
    return BADCATPARM;

  Status s; // changed to s, easier to type
  Record rec;
  RID rid;

  /** my code starts here **/
  HeapFileScan sc(RELCATNAME, s);
  CHKSTAT(s);
  sc.startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  s = sc.scanNext(rid);
  CHKSTAT(s); // 
  s = sc.getRecord(rec);
  CHKSTAT(s); //
  memcpy(record.relName, rec.data, rec.length);
  return OK;
}


const Status RelCatalog::addInfo(RelDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status s;
  
  /** my code starts here **/
  ifs = new InsertFileScan(RELCATNAME, s);
  CHKSTAT(s);
  Record rec = {(void*)(&record), sizeof(RelDesc)};
  s = ifs->insertRecord(rec, rid);
  CHKSTAT(s);
  delete ifs;
  ifs = NULL;
  return OK;
}

const Status RelCatalog::removeInfo(const string & relation)
{
  Status s;
  RID rid;
  HeapFileScan*  hfs;

  if (relation.empty()) return BADCATPARM;

  /** my code starts here **/
  hfs = new HeapFileScan(RELCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  CHKSTAT(s);
  s = hfs->scanNext(rid);
  CHKSTAT(s);
  hfs->deleteRecord();
  CHKSTAT(s);  
  delete hfs;
  hfs = NULL;
  return OK;
}


RelCatalog::~RelCatalog()
{
// nothing should be needed here
}


AttrCatalog::AttrCatalog(Status &status) :
	 HeapFile(ATTRCATNAME, status)
{
// nothing should be needed here
}


const Status AttrCatalog::getInfo(const string & relation, 
				  const string & attrName,
				  AttrDesc &record)
{

  Status s;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;

  if (relation.empty() || attrName.empty()) return BADCATPARM;
  
  /** my code starts here **/
  hfs = new HeapFileScan(ATTRCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  CHKSTAT(s);
  AttrDesc* pAttr = NULL;
  while(pAttr == NULL || strcmp(pAttr->attrName, attrName.c_str()) != 0){
    s = hfs->scanNext(rid);
    CHKSTAT(s);
    s = hfs->getRecord(rec);
    CHKSTAT(s);
    pAttr = (AttrDesc*)(rec.data);
  }
  memcpy((void*)&record, rec.data, rec.length);
  delete hfs;
  hfs = NULL;
  return OK;
}


const Status AttrCatalog::addInfo(AttrDesc & record)
{
  RID rid;
  InsertFileScan*  ifs;
  Status s;

  /** my code starts here**/
  ifs = new InsertFileScan(ATTRCATNAME, s);
  CHKSTAT(s);
  Record rec = {(void*)&(record), sizeof(AttrDesc)};
  s = ifs->insertRecord(rec, rid);
  CHKSTAT(s);

  delete ifs;
  ifs = NULL;
  return OK;
}


const Status AttrCatalog::removeInfo(const string & relation, 
			       const string & attrName)
{
  Status s;
  Record rec;
  RID rid;
  //AttrDesc record;
  HeapFileScan*  hfs;

  if (relation.empty() || attrName.empty()) return BADCATPARM;

  /** my code starts here **/
  hfs = new HeapFileScan(ATTRCATNAME, s);
  CHKSTAT(s);
  hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  CHKSTAT(s);
  AttrDesc* pAttr = NULL;
  while(pAttr == NULL || strcmp(pAttr->attrName, attrName.c_str()) != 0){
    s = hfs->scanNext(rid);
    CHKSTAT(s);
    s = hfs->getRecord(rec);
    CHKSTAT(s);
    pAttr = (AttrDesc*) rec.data;
  }
  s = hfs->deleteRecord();
  CHKSTAT(s);

  delete hfs;
  hfs = NULL;
  return OK;
}


const Status AttrCatalog::getRelInfo(const string & relation, 
				     int &attrCnt,
				     AttrDesc *&attrs)
{
  Status s;
  RID rid;
  Record rec;
  HeapFileScan*  hfs;

  if (relation.empty()) return BADCATPARM;

  /** my code starts here **/
  hfs = new HeapFileScan(RELCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  CHKSTAT(s);
  s = hfs->scanNext(rid);
  CHKSTAT(s);
  s = hfs->getRecord(rec);
  RelDesc* pRel = (RelDesc*) rec.data;
  attrCnt = pRel->attrCnt;
  attrs = new AttrDesc[attrCnt];
  delete hfs;
  hfs = new HeapFileScan(ATTRCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);  
  CHKSTAT(s);
  // retrieve all AttrDesc
  for(int i = 0; i < attrCnt; i++){
    s = hfs->scanNext(rid);
    CHKSTAT(s);
    s = hfs->getRecord(rec);
    CHKSTAT(s);
    memcpy(attrs+i, rec.data, rec.length);
  }

  delete hfs;
  hfs = NULL;
  return OK;
}


AttrCatalog::~AttrCatalog()
{
// nothing should be needed here
}

