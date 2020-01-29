#ifndef DBDRIVER_H_INCLUDED
#define DBDRIVER_H_INCLUDED




/*int getdbversion1(void);
int getdbversion2(void);
int insertintodb(void);
int createdynamically(void);*/

void createBMSTable();
void insertBMSEntry();
int createBMSdynamically(void);
int updatedbEntry_Value(int ivar);
int updatedbAllRow();


#endif // DBDRIVER_H_INCLUDED
