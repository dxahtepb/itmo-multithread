package com.dxahtepb.spark.logfilter;

import com.dxahtepb.spark.logfilter.job.ErrorFilterJob;
import com.dxahtepb.spark.logfilter.job.ErrorWindowedAggregationJob;
import com.dxahtepb.spark.logfilter.job.MethodCodeAggregationJob;
import com.dxahtepb.spark.logfilter.job.SparkJob;
import org.apache.commons.configuration.BaseConfiguration;
import org.apache.commons.configuration.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.spark.SparkConf;
import org.apache.spark.sql.SparkSession;

import java.io.IOException;
import java.net.URI;
import java.util.Arrays;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Main {
    private static final Logger LOG = Logger.getLogger(Main.class.getName());
    private static final String HDFS_URL = System.getenv("HADOOP_HDFS_NAMENODE");
    private static final String INPUT_PATH = HDFS_URL + "/";
    private static final String OUTPUT_PATH = HDFS_URL + "/out/results/";

    private static SparkSession initSpark() {
        SparkConf sparkConf = new SparkConf()
                .setMaster("yarn")
                .setAppName("NASA Logs Aggregation");
        return SparkSession
                .builder()
                .config(sparkConf)
                .getOrCreate();
    }

    public static int runJobs() {
        Configuration conf = new BaseConfiguration();
        conf.addProperty("input.dir", INPUT_PATH + "/logs");
        conf.addProperty("output.dir", OUTPUT_PATH);
        conf.addProperty("hdfs", HDFS_URL);

        try (SparkSession spark = initSpark();
             FileSystem fs = FileSystem.newInstance(URI.create(HDFS_URL), new org.apache.hadoop.conf.Configuration())) {
            fs.delete(new Path(OUTPUT_PATH), true);
            fs.mkdirs(new Path(OUTPUT_PATH));

            List<SparkJob> jobList = Arrays.asList(
                    ErrorFilterJob.create(spark, fs, conf),
                    MethodCodeAggregationJob.create(spark, fs, conf),
                    ErrorWindowedAggregationJob.create(spark, fs, conf)
            );

            for (SparkJob sparkJob : jobList) {
                String jobName = sparkJob.getClass().getSimpleName();
                LOG.info("Job started: " + jobName);
                try {
                    sparkJob.run();
                } catch (Exception ex) {
                    LOG.log(Level.SEVERE, "Job " + jobName + " failed: ", ex);
                }
                LOG.info("Job finished: " + jobName);
            }
        } catch (IOException ex) {
            LOG.log(Level.SEVERE, "HDFS error: ", ex);
            return 1;
        }
        return 0;
    }

    public static void main(String[] args) {
        int exitCode = runJobs();
        if (exitCode != 0) {
            System.exit(exitCode);
        }
    }
}
