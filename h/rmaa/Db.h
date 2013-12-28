#ifndef db_h
#define db_h

/************* DB-AMPLITUDE CONVERSIONS MODULE **************/

class DB {
public:
	DB();
	double	Amp2DB(double amp);		// Converts amplitude to decibels
	double	DB2Amp(double db);		// Converts decibels to amplitude
private:
	double	Amp2DB_Const;
	double	DB2Amp_Const;
};

#endif
