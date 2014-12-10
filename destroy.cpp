#include "catalog.h"

//
// Destroys a relation. It performs the following steps:
//
// 	removes the catalog entry for the relation
// 	destroys the heap file containing the tuples in the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::destroyRel(const string & relation)
{
  Status s;
  RID rid;
  Record rec;

  if (relation.empty() || 
      relation == string(RELCATNAME) || 
      relation == string(ATTRCATNAME))
    return BADCATPARM;

  /** my code starts here **/
  /** check if relation exists **/
  RelDesc rel;
  s = relCat->getInfo(relation, rel);
  if(s != OK){
    return RELNOTFOUND;
  }
  /** drop relation in attibute catalog **/
  attrCat->dropRelation(relation);
  /** remove information in relation catalog **/
  //delete from relCat
  HeapFileScan* hfs = new HeapFileScan(RELCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ); 
  CHKSTAT(s);
  s = hfs->scanNext(rid);
  CHKSTAT(s);
  s = hfs->getRecord(rec);
  CHKSTAT(s);
  RelDesc* pRd = (RelDesc*) rec.data;
  int attrCnt = pRd->attrCnt;
  s = removeInfo(relation);
  CHKSTAT(s);
  /** destroy heapfile **/
  s = destroyHeapFile(relation);
  CHKSTAT(s);
  return OK;
  // delete from relCat
  // HeapFileScan* hfs = new HeapFileScan(RELCATNAME, s);
  // CHKSTAT(s);
  // s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ); 
  // CHKSTAT(s);
  // s = hfs->scanNext(rid);
  // CHKSTAT(s);
  // s = hfs->getRecord(rec);
  // CHKSTAT(s);
  // RelDesc* pRd = (RelDesc*) rec.data;
  // int attrCnt = pRd->attrCnt;
  // s = removeInfo(relation);
  // CHKSTAT(s);
  // // delete from attrCat
  // delete hfs;
  // hfs = new HeapFileScan(ATTRCATNAME, s);
  // string* attrNames = new string[attrCnt];
  // CHKSTAT(s);
  // s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  // CHKSTAT(s);
  // for(int i = 0; i < attrCnt; i++){
  //   s = hfs->scanNext(rid);
  //   CHKSTAT(s);
  //   s = hfs->getRecord(rec);
  //   CHKSTAT(s);
  //   AttrDesc* pAttr = (AttrDesc*) rec.data;
  //   attrNames[i] = pAttr->attrName;
  // }
  // for(int i = 0; i < attrCnt; i++){
  //   s = attrCat->removeInfo(relation, attrNames[i]);
  //   CHKSTAT(s);
  // }
  // delete hfs;
  // hfs = NULL;
  // // destroy heapfile
  // s = destroyHeapFile(relation);
  // CHKSTAT(s);
  // return OK;
}


//
// Drops a relation. It performs the following steps:
//
// 	removes the catalog entries for the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status AttrCatalog::dropRelation(const string & relation)
{
  Status s;
  //AttrDesc *attrs;
  int attrCnt;
  Record rec;
  RID rid;

  if (relation.empty()) return BADCATPARM;

  /** my code starts here **/
  HeapFileScan* hfs = new HeapFileScan(RELCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ); 
  CHKSTAT(s);
  s = hfs->scanNext(rid);
  CHKSTAT(s);
  s = hfs->getRecord(rec);
  CHKSTAT(s);
  RelDesc* pRd = (RelDesc*) rec.data;
  attrCnt = pRd->attrCnt;
  // delete from attrCat
  delete hfs;
  hfs = new HeapFileScan(ATTRCATNAME, s);
  //attrs = new AttrDesc[attrCnt];
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  CHKSTAT(s);
  for(int i = 0; i < attrCnt; i++){
    s = hfs->scanNext(rid);
    CHKSTAT(s);
    //    s = getRecord(rec);
    //CHKSTAT(s);
    //memcpy(attrs+i, rec.data, rec.length);
    s = hfs->deleteRecord();
    CHKSTAT(s);
  }
  /*  for(int i = 0; i < attrCnt; i++){
    s = attrCat->removeInfo(relName, attrs[i].attrName);
    CHKSTAT(s);
    }*/
  delete hfs;
  hfs = NULL;
  return OK;


}


