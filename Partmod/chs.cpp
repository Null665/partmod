#include "chs.hpp"

CHS::CHS()
{
cylinder=0,head=0,sector=0,
spt=63,tpc=255;
}

CHS::CHS(int c,int h,int s)
{
cylinder=c,head=h,sector=s,

spt=63,tpc=255;
}

CHS::CHS(long long lba)
{
spt=63,tpc=255;
calc(lba);

}


void CHS::calc(long long lba)
{
long temp;

cylinder =lba / (tpc * spt);
temp     = lba % (tpc * spt);
head     = temp / spt;
sector   = temp % spt + 1;

}


bool CHS::SetCylinder(unsigned int cylinder)
{
   this->cylinder=cylinder;
   return true;
}

bool CHS::SetHead(unsigned int head)
{
   if(head>tpc) return false;
   this->head=head;
   return true;
}

bool CHS::SetSector(unsigned int sector)
{
   if(head>spt) return false;
   this->sector=sector;
   return true;
}


unsigned int CHS::GetCylinder()
{
  return this->cylinder;
}
unsigned int CHS::GetHead()
{
  return this->head;
}
unsigned int CHS::GetSector()
{
   return this->sector;
}


bool CHS::SetGeometry(int spt,int tpc)
{
  this->spt=spt;
  this->tpc=tpc;
return true;
}

int CHS::GetSPT() { return spt; }
int CHS::GetTPC() { return tpc; }

void CHS::SetSPT(int spt)
{
long long old_lba=ToLBA();
this->spt=spt;
calc(old_lba);
}
void CHS::SetTPC(int tpc)
{
long long old_lba=ToLBA();
this->tpc=tpc;
 calc(old_lba);

}


long long CHS::ToLBA()
{
long long lba= ( ( cylinder * tpc + head ) * spt ) + sector - 1;
return lba;
}






CHS &CHS::operator =(long long lba)
{
unsigned long temp;

cylinder =lba / (tpc * spt);
temp     = lba % (tpc * spt);
head     = temp / spt;
sector   = temp % spt + 1;

return *this;
}

bool CHS::operator ==(CHS &chs)
{
if(this->ToLBA()==chs.ToLBA())
    return true;

return false;
}



CHS &CHS::operator ++()
{
unsigned long temp;
long long lba=this->ToLBA()+1;

cylinder =lba / (tpc * spt);
temp     = lba % (tpc * spt);
head     = temp / spt;
sector   = temp % spt + 1;

return *this;
}

CHS &CHS::operator ++(int)
{
CHS ret=*this;

unsigned long temp;
long long lba=this->ToLBA()+1;

cylinder =lba / (tpc * spt);
temp     = lba % (tpc * spt);
head     = temp / spt;
sector   = temp % spt + 1;

return ret;
}

