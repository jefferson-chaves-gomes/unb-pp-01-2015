#ifndef MPISETTINGS_H_
#define MPISETTINGS_H_

class MPISettings
{
    int PROC_SIZE_;
    int PROC_ID_;

    MPISettings();
    static MPISettings& instance();

public:
    static const int PROC_MASTER = 0;
    static int PROC_SIZE();
    static int PROC_ID();
    static bool IS_PROC_ID_MASTER();
};

#endif
