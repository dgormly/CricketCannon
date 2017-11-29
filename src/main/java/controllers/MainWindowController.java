package controllers;

import com.fazecast.jSerialComm.SerialPort;
import com.jfoenix.controls.*;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeTableColumn;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;
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

    //TODO Implement Records table.
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

        /* Setup event handlers. */
        fireButton.setOnAction(e -> {
            fireCannon();
        });


        stopButton.setOnAction(e -> {
            stopCannon();
        });

        fileSelectButton.setOnAction(e -> {
            findFilePath();
        });

        // Load ports.
        List<String> comNames = new ArrayList<>();
        for (SerialPort serialPort : SerialPort.getCommPorts()) {
            comNames.add(serialPort.getDescriptivePortName());
        }

        comList = FXCollections.observableArrayList(comNames);
        usbCombo.setItems(comList);

        usbCombo.setOnAction(e -> {
            if (usbDao.getPort() != null && usbDao.getPort().isOpen()) {
                usbDao.getPort().closePort();
                usbDao.setSetup(false);
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

    /**
     * Fires the cannon if all parameters are valid.
     *
     * @return
     *      True if cannon fires
     *      False otherwise
     */
    private boolean fireCannon() {
            if (checkParams()) {
                usbDao.sendMessage("#Fire!");
                stopButton.setDisable(false);
                fireButton.setDisable(true);
                return true;
        } else {
            return false;
        }
    }


    /**
     * Stops the cannon sequence.
     *
     */
    private void stopCannon() {
        stopButton.setDisable(true);
        fireButton.setDisable(false);
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


    /**
     * Exports data to given CSV file.
     *
     * TODO Don't know if I am going to need this. Link at the end.
     *
     * @param content
     * @param file
     */
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


    /**
     * Checks if the given cannon parameters are valid.
     *
     * @return
     *      True if all parameters are valid
     *      False otherwise.
     */
    private boolean checkParams() {

        if (fileLocationTextField.getText() == "") return false;
        if (usbDao.getPort() == null) return false;
        if (fileLocationTextField.getText() == "") return false;
        if (!usbDao.isSetup()) return false;


        try {
            numBalls = Integer.parseInt(numBallsTextfield.getText());
            numShots = Integer.parseInt(numShotsTextField.getText());
            file = new File(fileLocationTextField.getText());
        } catch (Exception e) {
            return false;
        }

        if (numBalls <= 0) return false;
        if (numShots <= 0) return false;


        return true;
    }


    /**
     * Adds a new record to the data table and CSV file.
     *
     * @param record
     *      Record to add.
     */
    private void addRecord(ShotRecord record) {
        // TODO add record to table.

    }
}
