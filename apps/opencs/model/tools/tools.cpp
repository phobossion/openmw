
#include "tools.hpp"

#include <QThreadPool>

#include "../doc/state.hpp"
#include "../doc/operation.hpp"
#include "../doc/document.hpp"

#include "../world/data.hpp"
#include "../world/universalid.hpp"

#include "reportmodel.hpp"
#include "mandatoryid.hpp"
#include "skillcheck.hpp"
#include "classcheck.hpp"
#include "factioncheck.hpp"
#include "racecheck.hpp"
#include "soundcheck.hpp"
#include "regioncheck.hpp"
#include "birthsigncheck.hpp"
#include "spellcheck.hpp"
#include "referenceablecheck.hpp"
#include "scriptcheck.hpp"
#include "bodypartcheck.hpp"
#include "referencecheck.hpp"
#include "startscriptcheck.hpp"

CSMDoc::OperationHolder *CSMTools::Tools::get (int type)
{
    switch (type)
    {
        case CSMDoc::State_Verifying: return &mVerifier;
    }

    return 0;
}

const CSMDoc::OperationHolder *CSMTools::Tools::get (int type) const
{
    return const_cast<Tools *> (this)->get (type);
}

CSMDoc::OperationHolder *CSMTools::Tools::getVerifier()
{
    if (!mVerifierOperation)
    {
        mVerifierOperation = new CSMDoc::Operation (CSMDoc::State_Verifying, false);

        connect (&mVerifier, SIGNAL (progress (int, int, int)), this, SIGNAL (progress (int, int, int)));
        connect (&mVerifier, SIGNAL (done (int, bool)), this, SIGNAL (done (int, bool)));
        connect (&mVerifier,
            SIGNAL (reportMessage (const CSMWorld::UniversalId&, const std::string&, const std::string&, int)),
            this, SLOT (verifierMessage (const CSMWorld::UniversalId&, const std::string&, const std::string&, int)));

        std::vector<std::string> mandatoryIds; //  I want C++11, damn it!
        mandatoryIds.push_back ("Day");
        mandatoryIds.push_back ("DaysPassed");
        mandatoryIds.push_back ("GameHour");
        mandatoryIds.push_back ("Month");
        mandatoryIds.push_back ("PCRace");

        mVerifierOperation->appendStage (new MandatoryIdStage (mData.getGlobals(),
            CSMWorld::UniversalId (CSMWorld::UniversalId::Type_Globals), mandatoryIds));

        mVerifierOperation->appendStage (new SkillCheckStage (mData.getSkills()));

        mVerifierOperation->appendStage (new ClassCheckStage (mData.getClasses()));

        mVerifierOperation->appendStage (new FactionCheckStage (mData.getFactions()));

        mVerifierOperation->appendStage (new RaceCheckStage (mData.getRaces()));

        mVerifierOperation->appendStage (new SoundCheckStage (mData.getSounds()));

        mVerifierOperation->appendStage (new RegionCheckStage (mData.getRegions()));

        mVerifierOperation->appendStage (new BirthsignCheckStage (mData.getBirthsigns()));

        mVerifierOperation->appendStage (new SpellCheckStage (mData.getSpells()));

        mVerifierOperation->appendStage (new ReferenceableCheckStage (mData.getReferenceables().getDataSet(), mData.getRaces(), mData.getClasses(), mData.getFactions()));

        mVerifierOperation->appendStage (new ReferenceCheckStage(mData.getReferences(), mData.getReferenceables(), mData.getCells(), mData.getFactions()));

        mVerifierOperation->appendStage (new ScriptCheckStage (mDocument));

        mVerifierOperation->appendStage (new StartScriptCheckStage (mData.getStartScripts(), mData.getScripts()));

        mVerifierOperation->appendStage(
            new BodyPartCheckStage(
                mData.getBodyParts(),
                mData.getResources(
                    CSMWorld::UniversalId( CSMWorld::UniversalId::Type_Meshes )),
                mData.getRaces() ));

        mVerifier.setOperation (mVerifierOperation);
    }

    return &mVerifier;
}

CSMTools::Tools::Tools (CSMDoc::Document& document)
: mDocument (document), mData (document.getData()), mVerifierOperation (0), mNextReportNumber (0)
{
    // index 0: load error log
    mReports.insert (std::make_pair (mNextReportNumber++, new ReportModel));
    mActiveReports.insert (std::make_pair (CSMDoc::State_Loading, 0));
}

CSMTools::Tools::~Tools()
{
    if (mVerifierOperation)
    {
        mVerifier.abortAndWait();
        delete mVerifierOperation;
    }

    for (std::map<int, ReportModel *>::iterator iter (mReports.begin()); iter!=mReports.end(); ++iter)
        delete iter->second;
}

CSMWorld::UniversalId CSMTools::Tools::runVerifier()
{
    mReports.insert (std::make_pair (mNextReportNumber++, new ReportModel));
    mActiveReports[CSMDoc::State_Verifying] = mNextReportNumber-1;

    getVerifier()->start();

    return CSMWorld::UniversalId (CSMWorld::UniversalId::Type_VerificationResults, mNextReportNumber-1);
}

void CSMTools::Tools::abortOperation (int type)
{
    if (CSMDoc::OperationHolder *operation = get (type))
        operation->abort();
}

int CSMTools::Tools::getRunningOperations() const
{
    static const int sOperations[] =
    {
       CSMDoc::State_Verifying,
        -1
    };

    int result = 0;

    for (int i=0; sOperations[i]!=-1; ++i)
        if (const CSMDoc::OperationHolder *operation = get (sOperations[i]))
            if (operation->isRunning())
                result |= sOperations[i];

    return result;
}

CSMTools::ReportModel *CSMTools::Tools::getReport (const CSMWorld::UniversalId& id)
{
    if (id.getType()!=CSMWorld::UniversalId::Type_VerificationResults &&
        id.getType()!=CSMWorld::UniversalId::Type_LoadErrorLog)
        throw std::logic_error ("invalid request for report model: " + id.toString());

    return mReports.at (id.getIndex());
}

void CSMTools::Tools::verifierMessage (const CSMWorld::UniversalId& id, const std::string& message,
    const std::string& hint, int type)
{
    std::map<int, int>::iterator iter = mActiveReports.find (type);

    if (iter!=mActiveReports.end())
        mReports[iter->second]->add (id, message, hint);
}

