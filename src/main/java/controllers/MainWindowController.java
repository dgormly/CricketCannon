package controllers;

import com.fazecast.jSerialComm.SerialPort;
import com.jfoenix.controls.*;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeTableColumn;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import util.ShotRecord;
import util.UsbDao;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.*;

/**
 * Main controller responsible for handling the GUI.
 *
 * @author Daniel Gormly
 * @project Cricket Australia Cannon project
 *
 * University of Queensland
 *
 * Email: dgormly.gormly@uqconnect.edu.com
 */
public class MainWindowController implements Initializable {

    /* FXML Nodes */

    @FXML
    private JFXButton fireButton;
    @FXML
    private JFXButton stopButton;
    @FXML
    private JFXTextField numShotsTextField;
    @FXML
    private JFXTextField numBallsTextfield;
    @FXML
    private JFXComboBox<String> usbCombo;
    @FXML
    private JFXTextField fileLocationTextField;
    @FXML
    private JFXButton fileSelectButton;
//    @FXML
//    private JFXTreeTableView<ShotRecord> dataTreeTableView;
//    @FXML
//    private JFXTreeTableColumn<ShotRecord, String> ballNumCol;
//    @FXML
//    private JFXTreeTableColumn<ShotRecord, String> idCol;
//    @FXML
//    private JFXTreeTableColumn<ShotRecord, String> shotCol;
//    @FXML
//    private JFXTreeTableColumn<ShotRecord, String> timeCol1;
//    @FXML
//    private JFXTreeTableColumn<ShotRecord, String> timeCol2;

    private ObservableList<String> comList;
    private UsbDao usbDao = new UsbDao();
    // Settings
    private File file;
    private int numBalls;
    private int numShots;

    public MainWindowController() throws InterruptedException {
    }


    /**
     * A function which sets up the button and input field handlers.
     *
     * Ties all of the buttons to the desired method on action.
     *
     * @param location  The location used to resolve relative paths for the root object, or null if
     *                  the location is not known.
     * @param resources The resources used to localize the root object, or null if the root object
     *                  was not localized.
     */
    @Override
    public void initialize(URL location, ResourceBundle resources) {

        fireButton.setOnAction(e -> {
            fireCannon();
        });


        stopButton.setOnAction(e -> {
            stopCannon();
        });

        fileSelectButton.setOnAction(e -> {
            findFilePath();
        });


        List<String> comNames = new ArrayList<>();
        for (SerialPort serialPort : SerialPort.getCommPorts()) {
            comNames.add(serialPort.getDescriptivePortName());
        }

        comList = FXCollections.observableArrayList(comNames);
        usbCombo.setItems(comList);

        usbCombo.setOnAction(e -> {
            if (usbDao.getPort() != null && usbDao.getPort().isOpen()) {
                usbDao.getPort().closePort();
            }
            usbDao.setPort(usbCombo.getValue());
        });

//        TreeItem<ShotRecord> item1 = new TreeItem<>();
//        TreeItem<ShotRecord> item2 = new TreeItem<>();
//        //Creating the root element
//        final TreeItem<ShotRecord> root = new TreeItem<>();
//        root.setExpanded(true);
//
//        // Add to tree
//        //Adding tree items to the root
//        root.getChildren().setAll(item1, item2);
//        dataTreeTableView.setRoot(root);


    }

    private boolean fireCannon() {
        usbDao.sendMessage("#Fire!");
        if (checkParams()) {
            stopButton.setDisable(false);
            fireButton.setDisable(true);
            return true;
        } else {
            return false;
        }
    }


    private boolean stopCannon() {
        stopButton.setDisable(true);
        fireButton.setDisable(false);
        return true;
    }


    /**
     * Opens a FileChooser to retrieve the show sheets location.
     */
    @FXML
    private void findFilePath() {
        FileChooser chooser = new FileChooser();
        FileChooser.ExtensionFilter extenFilter = new FileChooser.ExtensionFilter("CSV Files (*.csv)", "*.csv");

        chooser.setTitle("Select Folder");
        chooser.getExtensionFilters().add(extenFilter);
        file = chooser.showSaveDialog(new Stage());

        if (file != null) {
            fileLocationTextField.setText(file.getPath());
        }
    }


    private void saveFile(String content, File file){
        try {
            FileWriter fileWriter = null;

            fileWriter = new FileWriter(file);
            fileWriter.write(content);
            fileWriter.close();
        } catch (IOException ex) {
            System.err.println("Failed to save file.");
        }

    }


    private boolean checkParams() {
        try {
            numBalls = Integer.parseInt(numBallsTextfield.getText());
            numShots = Integer.parseInt(numShotsTextField.getText());
        } catch (Exception e) {
            return false;
        }
        if (fileLocationTextField.getText() != "" && usbDao.getPort() != null) {
            return true;
        }
        return false;
    }


    private ShotRecord getShotData() {
        return null;
    }


    private void addRecord(ShotRecord record) {
        // TODO add record to table.

    }
}
