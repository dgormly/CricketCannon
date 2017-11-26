package util;

import com.jfoenix.controls.datamodels.treetable.RecursiveTreeObject;

import java.util.Date;

public class ShotRecord extends RecursiveTreeObject<ShotRecord> {

    private double time1;
    private double time2;
    private int uid;
    private int shotNum;
    private Date time;

    public ShotRecord(double time1, double time2, int uid, int shotNum, Date time) {
        this.time1 = time1;
        this.time2 = time2;
        this.uid = uid;
        this.shotNum = shotNum;
        this.time = time;
    }
}
