#include <unistd.h>
#include <fcntl.h>
#include "catalog.h"
#include "utility.h"


//
// Loads a file of (binary) tuples from a standard file into the relation.
// Any indices on the relation are updated appropriately.
//
// Returns:
// 	OK on success
// 	an error code otherwise
//

const Status UT_Load(const string & relation, const string & fileName)
{
  Status s;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;
  InsertFileScan * iFile;
  int width = 0;

  if (relation.empty() || fileName.empty() || relation == string(RELCATNAME)
      || relation == string(ATTRCATNAME))
    return BADCATPARM;

  // open Unix data file

  int fd;
  if ((fd = open(fileName.c_str(), O_RDONLY, 0)) < 0)
    return UNIXERR;
  Record rRec;
  // get relation data
  // check if relation exists
  s = relCat->getInfo(relation, rd);
  if(s != OK){
    return RELNOTFOUND;
  }
  // get relation info
  HeapFileScan* hfs = new HeapFileScan(RELCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  CHKSTAT(s);
  RID outRid;
  s = hfs->scanNext(outRid);
  CHKSTAT(s);
  s = hfs->getRecord(rRec);
  CHKSTAT(s);
  memcpy((void*)&rd, rRec.data, rRec.length);
  attrCnt = rd.attrCnt;
  // get total length of record
  hfs->endScan();  
  delete hfs;
  attrs = new AttrDesc[attrCnt];
  hfs = new HeapFileScan(ATTRCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  for(int i = 0; i < attrCnt; i++){
    s = hfs->scanNext(outRid);
    CHKSTAT(s);
    s = hfs->getRecord(rRec);
    CHKSTAT(s);
    memcpy((void*)(attrs+i), rRec.data, rRec.length);
    width += attrs[i].attrLen;
  }
  hfs->endScan();
  delete hfs;
  hfs = NULL;
  //printf("total length of record: %d", width);
  // start insertFileScan on relation
  iFile = new InsertFileScan(relation.c_str(), s);
  CHKSTAT(s);

  // allocate buffer to hold record read from unix file
  char *record;
  if (!(record = new char [width])) return INSUFMEM;

  int records = 0;
  int nbytes;
  Record rec;

  // read next input record from Unix file and insert it into relation
  while((nbytes = read(fd, record, width)) == width) {
    RID rid;
    rec.data = record;
    rec.length = width;
    if ((s = iFile->insertRecord(rec, rid)) != OK) return s;
    records++;
  }

  // close heap file and unix file
  delete iFile;
  iFile = NULL;
  if (close(fd) < 0) return UNIXERR;

  return OK;
}

