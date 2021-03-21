#include "mainwindow.h"
#include "ui_mainwindow.h"
//speechFunctions.h
//these are some speech functions that utilize the espeak C API
//you need to call speakInit at start of program and speakCleanup at the end
//written by: Bill Heaster
//TheCreator a7 ApexLogic |)0T net

#include <stdio.h>
#include <QProcess>
#include <QQueue>
#include <thread>
#include <string.h>
#include <espeak/speak_lib.h>
#include <unistd.h>

//don't delete this callback function.
int SynthCallback(short *wav, int numsamples, espeak_EVENT *events)
{

    return 0;
}
int speakInit()
{


    //must be called before any other functions
    //espeak initialize
    if(espeak_Initialize(AUDIO_OUTPUT_SYNCH_PLAYBACK,0,NULL,espeakINITIALIZE_PHONEME_EVENTS) <0)
    {
        puts("could not initialize espeak\n");
        return -1;
    }

    espeak_SetSynthCallback(SynthCallback);
    return 0;
}

void speakCleanup()
{
    espeak_Terminate();
}

//takes in a char array and length, will speak the words.
int speak(char const * cp, int len)
{
    espeak_ERROR speakErr;

    if((speakErr=espeak_Synth(cp, len, 0,(espeak_POSITION_TYPE)0,0,espeakCHARS_AUTO,NULL,NULL))!= EE_OK)
    {
        printf("error on synth\n");
        return -1;
    }


    return 0;
}
int speak(const QString& gwer)
{
    auto fe = gwer.toLatin1();
    return speak(fe.constData(), fe.length());
}

//changes the base pitch, accepts values within 0-100
int changePitch(int newVal)
{
    espeak_ERROR sErr;

    if(newVal < 0)
    {
        newVal = 0;
    }
    if(newVal > 100)
    {
        newVal = 100;
    }

    if((sErr=espeak_SetParameter(espeakPITCH, newVal, 0)) != EE_OK)
    {
        printf("error while setting espeak parameters\n");
        return -1;
    }
    return 0;
}


//changes the range of pitch within the voice accepts values within 0-100, 50 is normal
int changeRange(int newVal)
{
    espeak_ERROR sErr;

    if(newVal < 0)
    {
        newVal = 0;
    }
    if(newVal > 100)
    {
        newVal = 100;
    }

    if((sErr=espeak_SetParameter(espeakRANGE, newVal, 0)) != EE_OK)
    {
        printf("error while setting espeak parameters\n");
        return -1;
    }
    return 0;
}


//changes word speed values should be bwtween 80 and 450.
int changeWordSpeed(int newVal)
{
    espeak_ERROR sErr;

    if(newVal < 80)
    {
        newVal = 80;
    }
    if(newVal > 450)
    {
        newVal = 450;
    }

    if((sErr=espeak_SetParameter(espeakRATE, newVal, 0)) != EE_OK)
    {
        printf("error while setting espeak parameters\n");
        return -1;
    }
    return 0;
}

QQueue<QString> exercises;
QString congrat;
std::thread work;
// speechtest.c

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    speakInit();
    speak("Поздравляю");

    ui->setupUi(this);
    work = std::thread([&](){
//        int exnum = 5;
        while(true) {
            while(exercises.empty()) {
                std::this_thread::yield();
            }
            QString exercise = exercises.dequeue();
            ui->exname->setText(exercise+" ");
            speak(exercise + "for eleven seconds in 5 seconds");
            std::this_thread::sleep_for (std::chrono::seconds(5));

            for(int exn = 0; exn < 4; exn++) {
                ui->exname->setText(exercise+" ");
                speak(excercises[exn/4] + "for eleven seconds");
                for(int sec = 11; sec > 0; sec--) {
                    while(pause2) {
                        std::this_thread::yield();
                    };
                    ui->label->setNum(sec);
    //                    QProcess p;
    //                    p.start("play", {"/home/n/Downloads/Ensoniq-SQ-1-Side-Stick.wav"});

                    std::this_thread::sleep_for (std::chrono::seconds(1));
                }
                if(exn < 3 || !exercises.empty()) {
                    if(exn == 3) exercise = exercises.front();
                    ui->exname->setText(excercise+" ");
                    speak("rest for nineteen seconds");
                    for(int sec = 19; sec > 0; sec--) {
                        while(pause2) {
                            std::this_thread::yield();
                        };
                        ui->label->setNum(sec);
        //                    QProcess p;
        //                    p.start("play", {"/home/n/Downloa ds/Ensoniq-SQ-1-Side-Stick.wav"});

                        std::this_thread::sleep_for (std::chrono::seconds(1));
                    }
                }
            }
            if(exercises.empty()) speak(congrat);
        }
    });

//    work.detach();

}


MainWindow::~MainWindow()
{
    delete ui;
    speakCleanup();
}
bool pause2;
void MainWindow::on_pushButton_clicked()
{
    QStringList cexcercises  {
        "decline pushups",
        "lateral pushups",
        "shoulder tap pushups",
        "pushups",
        "incline pushups"};
    exercises.append(cexcercises);
    congrat = "congratulations. You finished your chest workout";
}

void MainWindow::on_toolButton_clicked(bool checked)
{
    pause2 = checked;
}
