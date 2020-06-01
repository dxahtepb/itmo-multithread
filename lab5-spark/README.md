## Lab 5 Spark Logs Aggregation

Requirements:
* Java 8+
* Maven
* docker / docker-compose
* curl
* gzip

```
# Run the cluster
docker-compose -f ./docker/docker-compose.yml up
# Cluster will take some time to bootstrap

# Download and unzip NASA logs
mkdir ./logs
cd ./logs
curl ftp://ita.ee.lbl.gov/traces/NASA_access_log_Jul95.gz  \
  --output log_Jul95.gz
gunzip log_Jul95.gz
curl ftp://ita.ee.lbl.gov/traces/NASA_access_log_Aug95.gz  \
  --output log_Aug95.gz
gunzip log_Aug95.gz
cd ..

# Put logs on HDFS
docker cp ./logs "namenode:/logs"
docker exec namenode hadoop fs -put /logs /

# Build the application with maven and deploy to the Spark container
mvn clean
mvn package
docker cp ./target/nasaLogsAggregation.jar "spark_master:/nasaLogsAggregation.jar"

# Run Spark application over YARN ResourceManager
docker exec spark_master spark-submit --master yarn /nasaLogsAggregation.jar

# Get results from HDFS
docker exec namenode hadoop fs -get /out/results /
docker cp "namenode:/results" ./
```

Jobs results are stored in the following format:
* Task 2
  * `requests_with_error` is the list of requests, one per line, that were finished with a `5XX` error code
  * `requests_with_error_count` contains a single number - count of such requests
* Task 3
  * `method_code_day_aggregation` is a CSV file with fields `Method,HttpCode,Day,Count` (without header)
* Task 4
  * `week_aggregation` is a CSV file with fields `WeekStartDate,WeekEndDate,Count` (without header)