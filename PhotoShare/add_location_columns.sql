ALTER TABLE Photos 
ADD COLUMN Latitude DOUBLE NULL,
ADD COLUMN Longitude DOUBLE NULL,
ADD COLUMN LocationName VARCHAR(255) NULL,
ADD COLUMN City VARCHAR(100) NULL,
ADD COLUMN Country VARCHAR(100) NULL;

CREATE INDEX IX_Photos_Location ON Photos (Latitude, Longitude);
CREATE INDEX IX_Photos_City ON Photos (City);
CREATE INDEX IX_Photos_Country ON Photos (Country);
