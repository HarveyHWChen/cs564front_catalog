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

  // get relation data
  HeapFileScan* hfs = new HeapFileScan(RELCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  CHKSTAT(s);
  RID outRid;
  s = hfs->scanNext(outRid);
  CHKSTAT(s);
  Record rRec;
  s = hfs->getRecord(rRec);
  CHKSTAT(s);
  memcpy((void*)&rd, rRec.data, rRec.length);
  attrCnt = rd.attrCnt;
  width = attrCnt * MAXSTRINGLEN;
  // start insertFileScan on relation
  iFile = new InsertFileScan(ATTRCATNAME, s);
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
  if (close(fd) < 0) return UNIXERR;

  return OK;
}

