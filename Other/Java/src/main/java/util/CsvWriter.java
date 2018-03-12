package util;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;


/**
 * @Author: Daniel Gormly
 * @Date: 20/11/2017
 *
 * CSV Writer is manages the data being written to a CSV file.
 *
 * TODO Write tests.
 *
 */
public class CsvWriter {

    public static void writeToFile(File file, List<ShotRecord> recordList) {

        try {
            FileWriter fileWriter = new FileWriter(file);

            for (ShotRecord record : recordList) {
                double time1 = record.getTime1();
                double time2 = record.getTime2();
                int uid = record.getUid();
                int shotNum = record.getShotNum();
                String time = record.getTime().toString();

                fileWriter.write(String.format("%d,%d,%d,%d,%d,%s\n", uid, time1, time2, shotNum, time));
                fileWriter.close();
            }
        } catch (IOException ex) {
            System.err.println("Failed to save file.");
        }
    }


}
