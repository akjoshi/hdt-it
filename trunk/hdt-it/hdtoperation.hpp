#ifndef HDTOPERATION_HPP
#define HDTOPERATION_HPP

#include <QProgressDialog>
#include <QMutex>
#include <QMutexLocker>
#include <HDTListener.hpp>
#include <HDT.hpp>
#include <SingleTriple.hpp>

#include "hdtcachedinfo.hpp"

class HDTOperation : public QObject, public hdt::ProgressListener {
    Q_OBJECT

private:
    hdt::HDT *hdt;
    HDTCachedInfo *hdtInfo;
    hdt::IteratorTripleString *iterator;
    unsigned int numResults;
    QString fileName;
    hdt::RDFNotation notation;
    string baseUri;
    char *errorMessage;
    bool isCancelled;
    QMutex isCancelledMutex;
    enum Operation {
        HDT_READ,
        RDF_READ,
        HDT_WRITE,
        RDF_WRITE,
        RESULT_EXPORT
    } op;
    QProgressDialog dialog;

public:
    HDTOperation(hdt::HDT *hdt);
    HDTOperation(hdt::HDT *hdt, HDTCachedInfo *hdtInfo);
    void saveToRDF(QString &fileName, hdt::RDFNotation notation);
    void saveToHDT(QString &fileName);
    void loadFromRDF(QString &fileName, hdt::RDFNotation notation, string &baseUri);
    void loadFromHDT(QString &fileName);
    void exportResults(QString &fileName, hdt::IteratorTripleString *iterator, unsigned int numResults, hdt::RDFNotation notation);
    void execute();
    int exec();
    void notifyProgress(float level, const char *section);
signals:
    void progressChanged(int progress);
    void messageChanged(QString message);
    void processFinished(int status);
private slots:
    void cancel();
};

#endif // HDTOPERATION_HPP